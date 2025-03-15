
import java.util.LinkedList;
import java.util.List;
import java.util.Scanner;



public class Main{
 private static Scanner scanner = new Scanner(System.in);

 public static void ex1() {
    // Read n from the command line
    int n = scanner.nextInt();


    for (int i = 0; i < n; i+=2) 
        System.out.println(i);
 }

 public static void ex2() {
    // Read n from the command line
    int a = scanner.nextInt();
    int b = scanner.nextInt();


    System.out.println("Max: " + Math.max(a, b));
 }

 public static void ex3() {
    // Read n from the command line
    int n = scanner.nextInt();

    long fact = 1;

    for (int i = 2; i <= n; i++) {
        fact *= i;
    }
    
    System.out.println(fact);
 }

 public static long ex4() {
    // Read n from the command line
    int n = scanner.nextInt();

    long sum = 0;

    for (int i = 3; i <= n; i++) {
        if (i % 3 == 0 || i % 5 == 0)
            sum += i;
    }
    return sum;
 }

 public static void ex5() {
    // Read n from the command line
    int n = scanner.nextInt();

    // Define a linked list to store the numbers

    List<Integer> even = new LinkedList<>();
    List<Integer> odd = new LinkedList<>();

    for (int i = 0; i < n; i++) {
       if (i % 2 == 0)
           even.add(i);
       else
           odd.add(i);
    }

    System.out.println(even);
    System.out.println(odd);
 }
 
 public static void ex7() {
    // Read n from the command line
    int n = scanner.nextInt();

    int x = 0, y = 1;

    for (int i = 0; i < n; i++) {
        System.out.print(y + " ");

        int temp = y;
        y += x;
        x = temp;
    }
 }

 public static void main(String args[]) {
    Main.ex1();
    // Main.ex2();
    Main.ex3();
    System.out.println(Main.ex4());
    Main.ex5();
    Main.ex7();
 }
}