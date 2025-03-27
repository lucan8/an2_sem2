import Data.Char

newtype Parser a = Parser { parse :: String -> [(a,String)] }

instance Monad Parser where
    return a = Parser (\cs -> [(a,cs)])
    p >>= f = Parser (\cs -> concat (map (\(a, cs') -> (parse (f a) cs')) (parse p cs)))

instance Applicative Parser where
    pure = return
    mf <*> ma = do
        f <- mf
        va <- ma
        return (f va)    

instance Functor Parser where              
    fmap f ma = pure f <*> ma  

item :: Parser Char
item = Parser (\cs -> case cs of
                "" -> []
                (c:cs) -> [(c,cs)])
  
zero :: Parser a
zero = Parser (const [])

psum :: Parser a -> Parser a -> Parser a
psum p q = Parser (\cs -> (parse p cs) ++ (parse q cs))

(<|>) :: Parser a -> Parser a -> Parser a
p <|> q = Parser (\cs -> case parse (psum p q) cs of
                                [] -> []
                                (x:xs) -> [x])

dpsum0 :: Parser [a] -> Parser [a]                       
dpsum0 p = p <|> (return [])

sat :: (Char -> Bool) -> Parser Char
sat p = do
            c <- item
            if p c then return c else zero

char :: Char -> Parser Char
char c = sat (c ==)

string :: String -> Parser String
string [] = return []
string (c:cs) = do
                    pc <- char c
                    prest <- string cs
                    return (pc : prest)

many0 :: Parser a -> Parser [a]
many0 p = dpsum0 (many1 p)

many1 :: Parser a -> Parser [a]
many1 p = do 
    a <- p
    aa <- many0 p
    return (a : aa)

spaces :: Parser String
spaces = many0 (sat isSpace)

token :: Parser a -> Parser a
token p = do
            spaces
            a <- p
            spaces
            return a

symbol :: String -> Parser String
symbol symb = token (string symb)

data AExp = Nu Int | Qid String | PlusE AExp AExp | TimesE AExp AExp
    deriving Show
    
aexp :: Parser AExp
aexp = plusexp <|> mulexp <|> npexp

npexp = parexp <|> qid <|> integer

parexp :: Parser AExp
parexp = do
            symbol "("
            p <- aexp
            symbol ")"
            return p

look :: Parser (Maybe Char)
look = Parser (\cs -> case cs of
      [] -> [(Nothing, [])]
      (c:cs') -> [(Just c, c:cs')]
    )

digit :: Parser Int
digit = do
          d <- sat isDigit
          return (digitToInt d)

integer :: Parser AExp
integer = do
                  spaces
                  s <- look
                  ss <- do
                            if s == (Just '-') then
                                                  do
                                                    item
                                                    return (-1)
                                               else return 1
                  d <- digitToInt <$> sat isDigit
                  if d == 0 
                    then 
                      return (Nu 0)
                    else 
                      do
                        ds <- many0 digit
                        return (Nu (ss*(asInt (d:ds))))
          where asInt ds = sum [d * (10^p) | (d, p) <- zip (reverse ds) [0..] ]

qid :: Parser AExp
qid = do
            char '\''
            a <- many1 (sat isLetter)
            return (Qid a)

plusexp :: Parser AExp
plusexp = do
            p <- npexp
            symbol "+"
            q <- npexp
            return (PlusE p q)

mulexp :: Parser AExp
mulexp = do
            p <- npexp
            symbol "*"
            q <- npexp
            return (TimesE p q)

data BExp = BE Bool | LE AExp AExp | NotE BExp | AndE BExp BExp
    deriving Show
    
bexp :: Parser BExp
bexp = lexp <|> notexp <|> andexp <|> npexpb

npexpb = parexpb <|> true <|> false

parexpb :: Parser BExp
parexpb = do
            symbol "("
            p <- bexp
            symbol ")"
            return p

true :: Parser BExp
true = do
            symbol "true"
            return (BE True)

false :: Parser BExp
false = do
            symbol "false"
            return (BE False)

lexp :: Parser BExp
lexp = do
            p <- npexp
            symbol "<="
            q <- npexp
            return (LE p q)

notexp :: Parser BExp
notexp =  do
            symbol "not"
            q <- npexpb
            return (NotE q)

andexp :: Parser BExp
andexp = do
            p <- npexpb
            symbol "&&"
            q <- npexpb
            return (AndE p q)
          
data Stmt = Skip | AtrE String AExp | Seq Stmt Stmt | IfE BExp Stmt Stmt | WhileE BExp Stmt
    deriving Show

stmt :: Parser Stmt
stmt = seqp <|> stmtneseq

stmtneseq :: Parser Stmt
stmtneseq = atre <|> ife <|> whileE <|> skip

atre :: Parser Stmt
atre = do
            spaces
            y <- qid
            case y of
                (Qid x) -> do
                            symbol ":="
                            a <- aexp
                            spaces
                            return (AtrE x a)
                _ -> zero

seqp :: Parser Stmt
seqp = do
            x <- stmtneseq
            rest x
      where rest x = (
                     do
                        symbol ";"
                        y <- stmtneseq
                        rest (Seq x y)
                     )
                     <|> return x

ife :: Parser Stmt
ife = do
          symbol "if"
          symbol "("
          b <- bexp
          symbol ")"
          symbol "{"
          s1 <- stmt
          symbol "}"
          symbol "else"
          symbol "{"
          s2 <- stmt
          symbol "}"
          return (IfE b s1 s2)

whileE :: Parser Stmt
whileE =  do
              symbol "while"
              symbol "("
              b <- bexp
              symbol ")"
              symbol "{"
              s1 <- stmt
              symbol "}"
              return (WhileE b s1)

skip :: Parser Stmt
skip = do
          symbol "skip"
          return Skip

sum_no = "'n:=100; 's:=0; 'i:=0; while ( ('i<= 'n)) { 's:='s+'i; 'i:= 'i+1} "

sum_no_p :: Stmt
sum_no_p = (fst.head) (parse stmt sum_no)

inf_cycle = "'n := 0; while (0 <= 0) {'n := 'n +1}"

inf_cycle_p :: Stmt
inf_cycle_p = (fst.head) (parse stmt inf_cycle)

recall :: String -> [(String, Int)] -> Int
recall s (h:t) = 
    if fst h == s
        then snd h
    else recall s t

update :: String -> Int -> [(String, Int)] -> [(String, Int)]
update s val [] = [(s, val)]
update s val (h:t) = 
    if fst h == s
        then (s, val) : t
    else h : update s val t

-- data AExp = Nu Int | Qid String | PlusE AExp AExp | TimesE AExp AExp
--     deriving Show
value :: AExp -> [(String, Int)] -> Int
value (Nu val) _ = val 
value (Qid var) l = recall var l
value (PlusE e1 e2) l = value e1 l + value e2 l
value (TimesE e1 e2) l = value e1 l * value e2 l

-- data BExp = BE Bool | LE AExp AExp | NotE BExp | AndE BExp BExp
--     deriving Show
valueb :: BExp -> [(String, Int)] -> Bool
valueb (BE b) l = b
valueb (LE e1 e2) l = value e1 l <= value e2 l
valueb (NotE b) l = not (valueb b l)
valueb (AndE b1 b2) l = valueb b1 l && valueb b2 l

-- data Stmt = Skip | AtrE String AExp | Seq Stmt Stmt | IfE BExp Stmt Stmt | WhileE BExp Stmt
--     deriving Show
bssos :: Stmt -> [(String, Int)] -> [(String, Int)]
bssos Skip l = l
bssos (AtrE var e) l = update var (value e l) l
bssos (Seq s1 s2) l = bssos s2 (bssos s1 l)
bssos (IfE b s1 s2) l = 
    if valueb b l
        then bssos s1 l
    else bssos s2 l
bssos (WhileE b s) l = 
    if not (valueb b l)
        then l
    else bssos (WhileE b s) (bssos s l)

test_bssos = recall "s" (bssos sum_no_p []) == 5050

-- data Stmt = Skip | AtrE String AExp | Seq Stmt Stmt | IfE BExp Stmt Stmt | WhileE BExp Stmt
--     deriving Show
sssos1 :: (Stmt, [(String, Int)]) -> (Stmt, [(String, Int)])
sssos1 (AtrE var exp, state) = (Skip, update var (value exp state) state)
sssos1 (Seq Skip s2, state) = (s2, state) 
sssos1 (Seq s1 s2, state) = 
    let 
        (new_s, new_state) = sssos1 (s1, state)
    in (Seq new_s s2, new_state)
sssos1 (IfE b s1 s2, state) =
    if valueb b state
        then (s1, state)
    else (s2, state)
sssos1 (WhileE b s, state) =
    (IfE b (Seq s (WhileE b s)) Skip, state)


sssos_star :: (Stmt, [(String, Int)]) -> [(Stmt, [(String, Int)])]
sssos_star (s1, s) = (s1, s):(sssos_plus (s1, s))

sssos_plus :: (Stmt, [(String, Int)]) -> [(Stmt, [(String, Int)])]
sssos_plus (Skip, s) = []
sssos_plus (s1, s) = (sssos_star . sssos1) (s1, s)

sssos_final_state :: (Stmt, [(String, Int)]) -> [(String, Int)]
sssos_final_state = snd . last . sssos_star

test_sssos = recall "s" (sssos_final_state (sum_no_p, [])) == 5050

prog = sum_no_p
inits = (prog, [])

test = (sssos_final_state inits) == (bssos prog [])