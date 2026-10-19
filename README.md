# PeptideRanker Academic Licence
Copyright (c) 2026 University College Dublin

Read the full Licence here:
[View the standalone licence file](./LICENCE.md)

COMMERCIAL USE requires a separate written licence from the Licensor. Enquiries: catherine.mooney@ucd.ie

<p align="center">
    <img
      src="./assets/peptideranker.png"
      alt="PeptideRanker"
      width="500"
    >
  </a>
</p>

## PeptideRanker is a software for predicting whether peptide sequences are likely to possess bioactivity.
Developed at University College Dublin (UCD) by Catherine Mooney and colleagues (catherine.mooney@ucd.ie).

## Download PeptideRanker

For users unfamiliar with Git or GitHub, PeptideRanker is available as a downloadable ZIP package.

📦 **[Download PeptideRanker v1.0.0 (ZIP)](https://github.com/shields-lab/peptideranker/releases/download/v1.0.0/PeptideRanker_Share.zip)**

### Getting Started
1. Click the download link above.
2. Extract the downloaded ZIP file.
3. Open the extracted folder.
4. Follow the included instructions to install and run PeptideRanker.
   
**Note:** Git installation or repository cloning is not required for the above.

## To set up:


In PeptideRanker.pl
set $pipeline_dir="."; to the full path to the PeptideRanker executable
(Models_Long.txt, Models_Short.txt and the Models folder need to be in the same location as the PeptideRanker executable) 


## To run:

To rank a list of peptides in order of predicted probability of being bioactive:

./PeptideRanker.pl input_file output_file
e.g. ./PeptideRanker.pl testfile.txt testfile.ranked
