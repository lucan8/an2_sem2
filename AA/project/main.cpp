#include <vector>
#include <iostream>
#include <random>
#include <algorithm>
#include <fstream>
#include <unordered_map>
#include <cassert>

// TODO: ADD CHECK FOR crossover in verify
// TODO: Make a vector printing function
// TODO: Encapsulate in a class

std::random_device rand_dev;
std::mt19937 gen(rand_dev());

class Chromosome;
class Function;
// Gets the relative probability for each value
// The values should be positive
std::vector<double> getRelativeProb(const std::vector<double>& values);

std::vector<double> getCumulativeIntervals(const std::vector<double>& probs);

int intervalBinSearch(double value, const std::vector<double>& partial_sums);

std::vector<bool> toBits(uint64_t n, int nr_bits);

uint64_t fromBits(const std::vector<bool>& bits);

void crossover(Chromosome& c1, Chromosome& c2, size_t cut_point, const Function& func);


// Asserting that the output from the homework example is the same as
// The output of the following functions
// fromBits, fitness, getRelativeProb, getCumulativeIntervals, intervalBinSearch
void verify();

class Function{
private:
    std::vector<double> func_coef;
    std::pair<double, double> func_domain;

    double interval_step;
public:
    Function(){}
    Function(const std::vector<double>& func_coef, double a, double b)
        : func_coef(func_coef), func_domain(a, b){}
    double fitness(double value) const;

    const std::pair<double, double>& getFuncDomain() const {return func_domain;}
    double getDomainLength() const {return func_domain.second - func_domain.first;}

    uint64_t getIntervalIndex(double value) const {
        return (value - func_domain.first) / interval_step;
    }

    double getIntervalStep() const {return interval_step;}
    void setIntervalStep(size_t nr_bits){ this->interval_step = getDomainLength() / (1 << nr_bits);}
    
    // Returns the interval value associated with the bit representation
    double fromBits(const std::vector<bool>& bits) const;

    // Returns the bit representation of the interval value
    std::vector<bool> toBits(double n, size_t nr_bits) const{return ::toBits(getIntervalIndex(n), nr_bits);}

    friend std::istream& operator>>(std::istream& in, Function& func);
};

class Chromosome{
private:
    std::vector<bool> bits;
    double value;

    std::ostream& printBits(std::ostream& out) const;
public:
    Chromosome(){}
    Chromosome(double value, const std::vector<bool>& bits): value(value), bits(bits){}

    double getValue() const {return value;}

    // Resets the value based on the (hopefully) changed bits
    // Should be used with crossover and mutation
    void resetValue(const Function& func);

    const std::vector<bool>& getBits() const{return bits;}
    std::string getBitsString() const;
    
    void mutation();
    
    // Swaps the bits of c1 with c2 starting at cut point and resets their value using func
    friend void crossover(Chromosome& c1, Chromosome& c2, size_t cut_point, const Function& func);

    bool operator==(const Chromosome& other) const{return abs(value - other.value) <= 1e-9 && bits == other.bits;}
    bool operator!=(const Chromosome& other) const {return !(*this == other);}
    friend std::ostream& operator<<(std::ostream& out, const Chromosome& chromo);
};

class GeneticAlgo{
private:
    std::ifstream fin;
    std::ofstream fout;

    std::vector<Chromosome> chromosomes;
    Function func;
    size_t precision;
    int nr_bits;

    double crossover_prob;
    double mutation_prob;

    size_t stage_count;
    size_t curr_stage = 0;

    // Should be used only once
    void initializePopulation(size_t population_size);

    void setChromosomes(const std::vector<double>& chromosomes);
public:
    GeneticAlgo(const std::string& in_file_name, const std::string& out_file_name);
    // Used for testing
    GeneticAlgo(const std::vector<double>& chromosomes, const Function& func, size_t precision, double crossover_prob, double mutation_prob, size_t stage_count)
        :   func(func), precision(precision), crossover_prob(crossover_prob), mutation_prob(mutation_prob), stage_count(stage_count){
            this->nr_bits = ceil(log2(func.getDomainLength() * pow(10, precision)));
            this->func.setIntervalStep(this->nr_bits);
            setChromosomes(chromosomes);
        }

    const std::vector<Chromosome>& getChromosomes() const{return chromosomes;}
    std::vector<double> getChromosomesValues() const;
    std::vector<double> getChromosomesFitness() const;
    Function getFunc() const {return func;}

    int getNrBits() const {return nr_bits;}
    size_t getPrecision() const {return precision;}
    // Determines and sets the new generation of chromosomes
    void naturalSelection();

    void crossover();
    void mutation();
    
    friend std::ostream& operator<<(std::ostream& out, const GeneticAlgo& g_algo);
    friend std::istream& operator>>(std::istream& in, GeneticAlgo& g_algo);

    void print(){fout << *this;}
};

int main(){
    std::string in_file_name = "data.in"; 
    std::string out_file_name = "data.out";

    GeneticAlgo test_algo(in_file_name, out_file_name);
    verify();
    test_algo.print();
    test_algo.naturalSelection();
    test_algo.crossover();

    // std::cout << test_algo.func.fitness(-0.914592) << '\n';

}

std::string Chromosome::getBitsString() const{
    std::string res = "";
    for (auto v : bits)
        res += std::to_string(v);
    return res;
}

GeneticAlgo::GeneticAlgo(const std::string& in_file_name, const std::string& out_file_name)
    : fin(in_file_name), fout(out_file_name){
    if (fin)
        fin >> *this;
    else
        throw std::runtime_error("[ERROR] GeneticAlgo: could not open file " + in_file_name);
}

// Determines and sets the new generation of chromosomes
void GeneticAlgo::naturalSelection(){
    std::vector<Chromosome> new_generation(chromosomes.size());

    // Get chromosomes fitness and send the best one to the next generation
    std::vector<double> chromo_fitnesses = getChromosomesFitness();
    int best_chromo_index = std::distance(chromo_fitnesses.begin(),
                                          std::max_element(chromo_fitnesses.begin(), chromo_fitnesses.end()));
    new_generation[0] = chromosomes[best_chromo_index];

    // Get and print chromosome selection probabilities
    std::vector<double> chromo_probs = getRelativeProb(chromo_fitnesses);
    fout << "Selection probabilities:\n";
    for (int i = 0; i < chromo_probs.size(); ++i)
        fout << "Chromosome " << i << ": "<< chromo_probs[i] << '\n';

    // Get and print the cumulative interval values
    std::vector<double> partial_sums = getCumulativeIntervals(chromo_probs);
    fout << "Partial sums:";
    // Print partial sums
    for (int i = 0; i < partial_sums.size(); ++i){
        if (i % 6 == 0)
            fout << '\n';
        fout << partial_sums[i] << " ";
    }
    fout << '\n';

    // Generate random numbers in the interval (0, 1)
    // Determine the partial sum interval it is from and the index of the interval is the selected chromo
    // The selected chromos will be part of the new generation
    // The first chromosome in the new gen is the best from the current one(why we are starting from 1)
    std::uniform_real_distribution<double> dist(0, 1);
    for (int i = 1; i < new_generation.size(); ++i){
        double gen_val = dist(gen);
        int chosen_interval = intervalBinSearch(gen_val, partial_sums);

        fout << "u= " << gen_val << " selecting chromosome " << chosen_interval << '\n';

        new_generation[i] = chromosomes[chosen_interval];
    }
    // Set new generation and print to file
    this->chromosomes = move(new_generation);
    fout << "New generation:\n";
    print();
}

void GeneticAlgo::crossover(){
    fout << "Crossover probability: " << crossover_prob << '\n';

    // Select chromo indexes for crossover if below the threshold(crossover_prob)
    std::vector<int> chosen_chromo_indexes;
    std::uniform_real_distribution<double> real_dist(0, 1);

    for (int i = 1; i < chromosomes.size(); ++i){
        double prob = real_dist(gen);
        fout << i << ": " << chromosomes[i].getBitsString() << " " << prob;

        if (prob <= crossover_prob){
            fout << "<=" << crossover_prob << " (participates)";
            chosen_chromo_indexes.push_back(i);
        }

        fout << '\n';
    }

    // Generate cut points for each pair of chosen chromos and crossover
    std::cout << nr_bits << "\n";
    std::uniform_int_distribution<size_t> int_dist(0, nr_bits - 1);

    for (int i = 0; i < chosen_chromo_indexes.size() - 1; i+=2){
        int cut_point = int_dist(gen);

        int chromo1_i = chosen_chromo_indexes[i],
            chromo2_i = chosen_chromo_indexes[i + 1];

        fout << "Crossover between chromo " << chromo1_i << " and " << chromo2_i << ":\n"
             << chromosomes[chromo1_i].getBitsString() << " " << chromosomes[chromo2_i].getBitsString()
             << " cut point " << cut_point << '\n';

        ::crossover(chromosomes[chromo1_i], chromosomes[chromo2_i], cut_point, func);

        fout << "Result: " << chromosomes[chromo1_i].getBitsString() << " " << chromosomes[chromo2_i].getBitsString() << '\n';
    }

    print();
}

void crossover(Chromosome& c1, Chromosome& c2, size_t cut_point, const Function& func){
    assert(c1.bits.size() == c2.bits.size());

    // Swaps bits starting at cut point
    for (int i = cut_point; i < c1.bits.size(); ++i){
        bool temp = c1.bits[i];
        c1.bits[i] = c2.bits[i];
        c2.bits[i] = temp;
    }

    // Resets the value of the chromosomes based on the changed bits
    c1.resetValue(func);
    c2.resetValue(func);
}

void Chromosome::resetValue(const Function& func){
    this->value = func.fromBits(this->bits);
}

double Function::fitness(double x) const{
    double result = 0;
    size_t coef_count = func_coef.size();

    for (int i = 0; i < coef_count; ++i)
        result += func_coef[i] * pow(x, coef_count - i - 1);
    return result;
}

void GeneticAlgo::initializePopulation(size_t population_size){
    std::uniform_real_distribution dist(func.getFuncDomain().first, func.getFuncDomain().second);

    chromosomes.resize(population_size);
    for (auto& chromo : chromosomes){
        double chromo_value = dist(gen);
        chromo = Chromosome(chromo_value, toBits(func.getIntervalIndex(chromo_value), nr_bits));
    }
}

// Returns the interval value associated with the bit representation
double Function::fromBits(const std::vector<bool>& bits) const{
    double interval_index = ::fromBits(bits);
    return func_domain.first + interval_index * interval_step;
}

std::ostream& operator<<(std::ostream& out, const GeneticAlgo& g_algo){
    for (int i = 0; i < g_algo.chromosomes.size(); ++i)
        out << i << ": " << g_algo.chromosomes[i] 
            << " f(value): " << g_algo.func.fitness(g_algo.chromosomes[i].getValue()) << '\n';
    
    return out;
}

std::ostream& operator<<(std::ostream& out, const Chromosome& chromo){
    chromo.printBits(out) << " value: " << chromo.value;

    return out;
}

std::ostream& Chromosome::printBits(std::ostream& out) const{
    for (auto b : bits)
        out << b;
    return out; 
}

std::istream& operator>>(std::istream& in, Function& func){
    // First line: grade of the function
    int func_grade;
    in >> func_grade;

    func.func_coef.resize(func_grade);
    for (int i = 0; i < func_grade; ++i)
        in >> func.func_coef[i];
    
    // Next line: function domain
    in >> func.func_domain.first >> func.func_domain.second;

    return in;
}

std::istream& operator>>(std::istream& in, GeneticAlgo& g_algo){
    in >> g_algo.func;
    in >> g_algo.precision;

    int population_size;
    in >> population_size;
    
    g_algo.nr_bits = ceil(log2(g_algo.func.getDomainLength() * pow(10, g_algo.precision)));
    g_algo.func.setIntervalStep(g_algo.nr_bits);
    g_algo.initializePopulation(population_size);

    in >> g_algo.crossover_prob >> g_algo.mutation_prob;
    in >> g_algo.stage_count;

    return in;
}

std::vector<double> GeneticAlgo::getChromosomesValues() const{
    std::vector<double> result(chromosomes.size());
    std::transform(chromosomes.begin(), chromosomes.end(), result.begin(), [](const Chromosome& chromo){
        return chromo.getValue();
    });
    return result;
}

std::vector<double> GeneticAlgo::getChromosomesFitness() const{
    std::vector<double> result(chromosomes.size());
    std::transform(chromosomes.begin(), chromosomes.end(), result.begin(), [this](const Chromosome& chromo){
        return this->func.fitness(chromo.getValue());
    });
    return result;
}

void GeneticAlgo::setChromosomes(const std::vector<double>& chromosomes){
    this->chromosomes.resize(chromosomes.size());
    for (int i = 0; i < chromosomes.size(); ++i)
        this->chromosomes[i] = Chromosome(chromosomes[i], func.toBits(chromosomes[i], nr_bits));
}

// Gets the relative probability for each value
// The values should be positive
std::vector<double> getRelativeProb(const std::vector<double>& values){
    // Making sure values are positive
    for (const auto& val : values)
        assert(val > 0);
    
    double chromo_sum = std::reduce(values.begin(), values.end());
    
    std::vector<double> result(values.size());
    for (int i = 0; i < result.size(); ++i)
        result[i] = values[i] / chromo_sum;
    
    return result;
}

std::vector<double> getCumulativeIntervals(const std::vector<double>& probs){
    // Making sure the values given are valid probabilities
    for (const auto& prob : probs)
        assert(prob >= 0 && prob <= 1);
    
    std::vector<double> result(probs.size() + 1);
    std::partial_sum(probs.begin(), probs.end(), result.begin() + 1);
    
    assert(result.front() == 0.0);
    assert(abs(result.back() - 1.0) <= 1e-9);

    return result;
}

int intervalBinSearch(double value, const std::vector<double>& partial_sums){
    if (value < partial_sums.front() || value > partial_sums.back())
        throw std::invalid_argument("[ERROR]: intervalBinSeach: Argument value = " + std::to_string(value) + " should be between " + std::to_string(partial_sums.front()) + ", " + std::to_string(partial_sums.back()));
    
    if (partial_sums.size() < 2)
        throw std::invalid_argument("[ERROR]: intervalBinSearch: Argument partial_sums must have at least 2 elements");

    int i = 0, j = partial_sums.size() - 1;

    while (i < j){
        int mid = (i + j) / 2;
        if (value <= partial_sums[mid])
            if (value >= partial_sums[mid - 1])
                return mid - 1;
            else
                j = mid - 1;
        else if (value <= partial_sums[mid + 1])
                return mid;
            else
                i = mid + 1;
    }

    return -1;
}

uint64_t fromBits(const std::vector<bool>& bits){
    uint64_t res = 0;
    size_t b_size = bits.size();
    for (int i = 0; i < b_size; ++i)
        res += bits[i] << (b_size - i - 1);
    return res;
}

std::vector<bool> toBits(uint64_t n, int nr_bits){
    std::vector<bool> result;
    result.reserve(10);

    while (n){
        result.push_back(n % 2);
        n = n >> 1;
    }

    while (result.size() != nr_bits)
        result.push_back(0);

    std::reverse(result.begin(), result.end());

    return result;
}

// Taking test input and output from homework and checking if it matches
// The output of the following functions
// fromBits, fitness, getRelativeProb, getCumulativeIntervals, intervalBinSearch
void verify(){
    std::vector<std::vector<bool>> bits = {
        {0,0,0,0,0,1,1,1,0,1,0,0,1,0,0,1,1,1,0,0,0,1},
        {0,0,1,0,1,0,0,1,0,0,1,1,1,0,1,1,1,1,1,1,0,0},
        {0,1,0,0,0,0,0,0,1,0,1,0,0,1,0,1,1,0,1,1,1,1},
        {1,1,0,1,0,0,1,1,1,0,0,0,1,1,0,1,1,0,1,1,0,1},
        {1,0,0,1,1,1,0,0,1,0,0,1,1,1,0,0,1,1,0,0,0,1},
        {1,0,1,1,1,1,1,0,0,1,0,0,0,0,1,1,0,0,0,1,0,0},
        {0,1,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,1,1,1,1},
        {0,0,0,0,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,0,1,0},
        {0,1,0,1,1,1,0,1,1,1,1,0,1,0,0,1,0,0,1,1,1,1},
        {0,0,1,1,1,0,1,0,1,0,1,1,0,1,1,0,0,0,1,0,0,1},
        {1,1,0,0,1,1,0,1,1,1,0,1,0,0,1,0,0,1,1,1,1,1},
        {0,1,1,1,1,1,1,1,1,0,0,0,1,1,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,0,0,0,0,1,0,0,1,1,0,0,0,0,0,0,0},
        {0,1,1,1,0,1,0,0,0,0,0,0,0,1,0,1,1,0,0,1,0,0},
        {1,1,0,0,0,0,0,0,0,1,1,1,0,1,1,1,0,0,0,1,0,1},
        {1,0,1,1,0,1,0,1,0,1,0,0,0,0,0,0,1,0,1,1,0,1},
        {1,1,0,1,0,1,1,1,0,1,0,1,1,0,1,1,0,1,0,1,0,1},
        {1,1,0,1,1,0,1,1,1,0,1,0,0,0,1,0,0,1,1,1,1,1},
        {0,0,1,0,0,1,0,1,0,1,1,0,0,1,0,1,1,0,1,1,1,0},
        {1,0,1,1,1,0,1,1,0,0,0,0,0,0,0,1,0,0,1,1,1,1}
    };
    
    std::vector<double> x_values = {
        -0.914592, -0.516787, -0.246207, 1.480791, 0.835307, 1.229633, 0.133068, 
        -0.897179, 0.100578, -0.311975, 1.411980, 0.404924, 1.954865, 0.359503, 
        1.255452, 1.124764, 1.527482, 1.573845, -0.562311, 1.191435
    };

    std::vector<Chromosome> chromos(x_values.size());
    for (int i = 0; i < x_values.size(); ++i)
        chromos[i] = Chromosome(x_values[i], bits[i]);
    
    std::vector<double> f_values = {
        0.24892945287918744, 1.2161433551687346, 1.69317538311855, 1.288048141479274, 
        2.1375690008662946, 1.7176358177971365, 2.115360642895274, 0.29789055630549277, 
        2.0904619502816644, 1.590697249227337, 1.4182933275624994, 2.240960465910953, 
        0.13336723106734194, 2.230260491735224, 1.6792930285557333, 1.8596696522000606, 
        1.1942809038290045, 1.0968573281925882, 1.1214962076226662, 1.771918100644858
    };

    Function test_func(std::vector<double>({-1, 1, 2}), -1, 2);
    GeneticAlgo test_algo(x_values, test_func, 6, 0.25, 0.1, 50);
    test_func = test_algo.getFunc();

    assert(test_algo.getNrBits() == bits[0].size());

    // Assert chromosome creation and fitness calculation
    const std::vector<Chromosome>& test_algo_chromos = test_algo.getChromosomes(); 
    const std::vector<double> test_algo_fitnesses = test_algo.getChromosomesFitness();

    // Assert fromBits function
    for (int i = 0; i < chromos.size(); ++i){
         double from_bits_value = test_func.fromBits(chromos[i].getBits());
         if (abs(from_bits_value - chromos[i].getValue()) > 1e-6)
             std::cerr << "[ASSERT FAILED]: From bits failed for chromosome " << i << " \n" << from_bits_value << "!=" << chromos[i].getValue() << '\n';
    }

    // Assert toBits function
    for (int i = 0; i < chromos.size(); ++i)
        if (test_func.toBits(chromos[i].getValue(), test_algo.getNrBits()) != chromos[i].getBits())
            std::cerr << "[ASSERT FAILED]: To bits failed for chromosome " << i << '\n';
    
    // Assert interval index calculation
    for (int i = 0; i < test_algo_chromos.size(); ++i){
        int test_algo_c_index = test_func.getIntervalIndex(test_algo_chromos[i].getValue());
        int c_index = test_func.getIntervalIndex(chromos[i].getValue());

        if (test_algo_c_index != c_index)
            std::cerr << "[ASSERT FAILED]: Interval index " << i << " different\n" << test_algo_c_index << "!=" << c_index << '\n';
    }

    // Assert chromosome creation
    for (int i = 0; i < test_algo_chromos.size(); ++i){
        if (test_algo_chromos[i] != chromos[i])
            std::cerr << "[ASSERT FAILED]: Chromosome " << i << " different\n" << test_algo_chromos[i].getValue()
                      << "!=" << chromos[i].getValue()<< '\n' << test_algo_chromos[i].getBitsString() << "!=" << chromos[i].getBitsString() << '\n';
    }

    // Assert fitness calculation
    for (int i = 0; i < test_algo_fitnesses.size(); ++i)
        if (abs(test_algo_fitnesses[i] - f_values[i]) > 1e-6)
            std::cerr << "[ASSERT FAILED]: Fitness " << i << " different\n" << test_algo_fitnesses[i] << "!=" << f_values[i] << '\n';

    // Vector of probabilities
    std::vector<double> chromo_probabilities = {
        0.0085418577836996, 0.041731195181167476, 0.058100249521212656,
        0.044198562748676756, 0.07334933732050686, 0.05893959396975151,
        0.07258727146930355, 0.010221927287582215, 0.07173288847506332,
        0.05458377673941411, 0.048667844481579396, 0.07689715048702854,
        0.004576412745083748, 0.07652998759552974, 0.05762388524608598,
        0.06381339576343534, 0.0409809988986984, 0.037637970107847966,
        0.03848343777592496, 0.06080225640240799
    };

    std::vector<double> test_probs = getRelativeProb(test_algo.getChromosomesFitness());
    // Assert chromosome selection probability
    for (int i = 0; i < test_probs.size(); ++i)
        if (abs(test_probs[i] - chromo_probabilities[i]) > 1e-6)
            std::cerr << "[ASSERT FAILED]: Chromosome probability " << i << " different\n" << test_probs[i] << "!=" << chromo_probabilities[i] << '\n';

    // Vector of cumulative probability intervals
    std::vector<double> cumulative_intervals = {
        0.0, 0.0085418577836996, 0.050273052964867074, 0.10837330248607974,
        0.1525718652347565, 0.22592120255526335, 0.28486079652501484,
        0.3574480679943184, 0.3676699952819006, 0.4394028837569639,
        0.493986660496378, 0.5426545049779574, 0.6195516554649859,
        0.6241280682100697, 0.7006580558055995, 0.7582819410516854,
        0.8220953368151208, 0.8630763357138193, 0.9007143058216672,
        0.9391977435975922, 1.0
    };

    std::vector<double> test_partial_sums = getCumulativeIntervals(test_probs);
    // Assert selection intervals calculation
    for (int i = 0; i < test_partial_sums.size(); ++i)
        if (abs(test_partial_sums[i] - cumulative_intervals[i]) > 1e-6)
            std::cerr << "[ASSERT FAILED]: Cumulative interval " << i << " different\n" << test_partial_sums[i] << "!=" << cumulative_intervals[i] << '\n';

    // Unordered map for u_value to chromosome index
    std::unordered_map<double, int> u_map = {
        {0.6034720442729782, 12}, {0.17892824175281696, 5},
        {0.6867737986711382, 14}, {0.7176059630936646, 15},
        {0.5619562065293205, 12}, {0.24081572433510423, 6},
        {0.5408716518401836, 11}, {0.43771824679891214, 9},
        {0.7390763060836184, 15}, {0.8447467542582436, 17},
        {0.7683657918169965, 16}, {0.9907124915454842, 20},
        {0.8395083889834671, 17}, {0.8113925195824372, 16},
        {0.8789773676106979, 18}, {0.9658384924654928, 20},
        {0.0756221139277069, 3}, {0.03335884865931882, 2},
        {0.26608038821171887, 6}, {0.5492264660581757, 12}
    };

    // Assert intervalBinSearch function
    for (const auto& p : u_map){
        int index = intervalBinSearch(p.first, test_partial_sums) + 1;
        if (index != p.second)
            std::cerr << "[ASSERT FAILED]: IntervalBinSearch: " << index << "!=" << p.second << '\n';
    }
}

