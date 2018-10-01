#include "subset.h"


void subset(Parameters& parameters) {

    /* The subset function parses through a file generated by process_reads and outputs sequences with the following criteria:
     * - Found in M males with min_males <= M <= max_males
     * - Found in F females with min_females <= F <= max_females
     */

    std::unordered_map<std::string, bool> popmap = load_popmap(parameters);

    std::string par = "input_file_path";
    std::ifstream input_file;
    input_file.open(parameters.get_value_from_name<std::string>(par));

    par = "min_cov";
    int min_cov = parameters.get_value_from_name<int>(par) - 1; // -1 allows comparison with > instead of >=

    par = "min_males";
    int min_males = parameters.get_value_from_name<int>(par) - 1; // -1 allows comparison with > instead of >=

    par = "min_females";
    int min_females = parameters.get_value_from_name<int>(par) - 1; // -1 allows comparison with > instead of >=

    par = "max_males";
    int max_males = parameters.get_value_from_name<int>(par) + 1; // +1 allows comparison with < instead of <=

    par = "max_females";
    int max_females = parameters.get_value_from_name<int>(par) + 1; // +1 allows comparison with < instead of <=

    par = "min_individuals";
    int min_individuals = parameters.get_value_from_name<int>(par) - 1; // -1 allows comparison with > instead of >=

    par = "max_individuals";
    int max_individuals = parameters.get_value_from_name<int>(par) + 1; // +1 allows comparison with < instead of <=

    par = "output_format";
    bool fasta = parameters.get_value_from_name<std::string>(par) == "fasta";

    if (input_file) {

        par = "output_file_path";
        std::ofstream output_file;
        output_file.open(parameters.get_value_from_name<std::string>(par));

        // First line is the header. The header is parsed to get the sex of each field in the table.
        std::vector<std::string> line;
        std::string temp = "";
        std::getline(input_file, temp);
        if (not fasta) output_file << temp << "\n"; // Copy the header line to the subset output file
        line = split(temp, "\t");

        // Map with column number --> index of sex_count (0 = male, 1 = female, 2 = no sex)
        std::unordered_map<uint, uint> sex_columns;

        // Detection of individuals is based on the popmap, so individuals without sex should still be in the popmap
        for (uint i=0; i<line.size(); ++i) {
            if (popmap.find(line[i]) != popmap.end()) {
                if (popmap[line[i]]) {
                    sex_columns[i] = 0; // Male --> column 0
                } else {
                    sex_columns[i] = 1; // Female --> column 1
                }
            } else {
                sex_columns[i] = 2; // First and second columns (id and sequence) are counted as no sex
            }
        }

        // Define variables used to read the file
        char buffer[65536];
        std::string temp_line;
        uint k = 0, field_n = 0;
        int sex_count[3] = {0, 0, 0}; // Index: 0 = male, 1 = female, 2 = no sex
        int n_individuals = 0;

        do {

            // Read a chunk of size given by the buffer
            input_file.read(buffer, sizeof(buffer));
            k = input_file.gcount();

            for (uint i=0; i<k; ++i) {

                // Read the buffer character by character
                switch(buffer[i]) {

                case '\r':
                    break;
                case '\t':  // New field
                    if (sex_columns[field_n] != 2 and std::stoi(temp) > min_cov) ++sex_count[sex_columns[field_n]];  // Increment the appropriate counter
                    temp = "";
                    temp_line += buffer[i];
                    ++field_n;
                    break;
                case '\n':  // New line (also a new field)
                    if (sex_columns[field_n] != 2 and std::stoi(temp) > min_cov) ++sex_count[sex_columns[field_n]];  // Increment the appropriate counter
                    n_individuals = sex_count[0] + sex_count[1] + sex_count[2];
                    if (sex_count[0] > min_males and sex_count[0] < max_males and sex_count[1] > min_females and sex_count[1] < max_females and n_individuals > min_individuals and n_individuals < max_individuals) {
                        if (fasta) {
                            line = split(temp_line, "\t");
                            output_file << ">" << line[0] << "_" << sex_count[0] << "M_" << sex_count[1] << "F_cov:" << min_cov + 1 << "\n" << line[1] << "\n";
                        } else {
                            output_file << temp_line << "\n";
                        }
                    }
                    // Reset variables
                    temp = "";
                    temp_line = "";
                    field_n = 0;
                    sex_count[0] = 0;
                    sex_count[1] = 0;
                    break;
                default:
                    temp += buffer[i];
                    temp_line += buffer[i];
                    break;
                }
            }

        } while (input_file);

        output_file.close();
        input_file.close();
    }
}
