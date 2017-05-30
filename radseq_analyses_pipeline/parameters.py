import os


class Parameters:

    def __init__(self, root_dir='', files_dir='', output_dir='output',
                 results_dir='results', error_threshold=0.1,
                 frequencies_file='frequencies_data.tsv',
                 haplotypes_file='haplotypes_data.tsv',
                 alleles_file='alleles_data.tsv',
                 neomales_file='neomales_data.tsv',
                 n_threads=1):

        self.root_dir = root_dir
        self.files_dir = files_dir
        self.output_dir = output_dir
        self.results_dir = results_dir
        self.error_threshold = error_threshold
        self.frequencies_file = os.path.join(output_dir, frequencies_file)
        self.haplotypes_file = os.path.join(output_dir, haplotypes_file)
        self.alleles_file = os.path.join(output_dir, alleles_file)
        self.neomales_file = os.path.join(output_dir, neomales_file)
        self.n_threads = n_threads
