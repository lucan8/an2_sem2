#include <vector>
#include <iostream>
// TODO: Track this on github
class Chromosomes{
private:
    std::vector<bool> bits;
public:
    void mutation();
    // This changes both chromosomes
    friend void crossover(Chromosomes& chromo1, Chromosomes& chromo2);

    friend std::ostream& operator<<(std::ostream& out, const Chromosomes& chromo);
};

class GeneticAlgo{
private:
    std::vector<Chromosomes> chromosomes;
    std::vector<double> func_coef;
    std::pair<double, double> func_domain;
    size_t precision;
    double crossover_prob;
    double mutation_prob;
    size_t stage_count;
    size_t curr_stage;

    void initializePopulation(size_t population_size);
public:
    double fitness(double x) const{
        double result = 0;
        for (double coef : func_coef)
            result += coef * x;
        return result;
    }

    void naturalSelection();
    void crossover();
    void mutation();

    friend std::ostream& operator<<(std::ostream& out, const GeneticAlgo& g_algo);
    friend std::istream& operator>>(std::istream& in, GeneticAlgo& g_algo);
};

std::istream& operator>>(std::istream& in, GeneticAlgo& g_algo){
    // First line: grade of the function
    int func_grade;
    in >> func_grade;

    // Next func_grade lines: the coef of the polinomial
    std::fill(g_algo.func_coef.begin(), g_algo.func_coef.end(), 0);
    for (int i = 0; i < func_grade; ++i)
        in >> g_algo.func_coef[i];
    
    // Next line: function domain
    in >> g_algo.func_domain.first >> g_algo.func_domain.second;
    
    // Next line: Interval precision
    in >> g_algo.precision;

    // Next line: The initial number of chromosomes
    int population_size;
    in >> population_size;

    g_algo.initializePopulation(population_size);

    // Next line: Crossover and mutation probabilities
    in >> g_algo.crossover_prob >> g_algo.mutation_prob;

    in >> g_algo.stage_count;
}