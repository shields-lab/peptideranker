# PeptideRanker Academic Licence
Copyright (c) 2026 University College Dublin

Commercial Use requires a separate written licence from the Licensor. Enquiries: catherine.mooney@ucd.ie


# Read the full Licence here
[View the standalone licence file](./LICENCE.md)

<p align="center">
    <img
      src="./assets/peptideranker.png"
      alt="PeptideRanker"
      width="500"
    >
  </a>
</p>

## PeptideRanker is a web server for predicting whether peptide sequences are likely to possess bioactivity.

## To set up:


In PeptideRanker.pl
set $pipeline_dir="."; to the full path to the PeptideRanker executable
(Models_Long.txt, Models_Short.txt and the Models folder need to be in the same location as the PeptideRanker executable) 


## To run:

To rank a list of peptides in order of predicted probability of being bioactive:

./PeptideRanker.pl input_file output_file
e.g. ./PeptideRanker.pl testfile.txt testfile.ranked
