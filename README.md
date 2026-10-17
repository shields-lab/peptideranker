# PeptideRanker Academic Licence
# [PeptideRanker](https://github.com/indrani2611/peptideranker)

[View the standalone licence file](./LICENCE.md)
[View the standalone licence file](./LICENCE.md)

Copyright (c) 2026 University College Dublin

## 1. Definitions

"Software" means PeptideRanker, including its source code, compiled
binaries, models, documentation, and any copies or derivatives of them.

"Academic Use" means use of the Software solely for research, teaching,
private study, or publication of non-commercial academic work, by an
individual affiliated with a recognised university, research institute,
or non-profit educational body.

"Commercial Use" means any use intended for or directed toward
commercial advantage or monetary compensation, including use in a
product, service, or process sold or licensed to others, use by or for
a for-profit organisation, and use in clinical, diagnostic, or other
decision-making for a fee.

## 2. Grant of Licence
Subject to this Licence, the Licensor grants you a non-exclusive,
non-transferable, royalty-free licence to use, copy, modify, and
distribute the Software, solely for Academic Use.

## 3. Restrictions

You may not:
(a) use the Software for Commercial Use;
(b) sublicense, sell, or rent the Software;
(c) distribute the Software except under this Licence, with this
    Licence text included in full;
(d) remove or alter copyright notices or this Licence.

## 4. Attribution

If you publish results produced with the Software, you are requested,
though not required, to cite the Software and its associated publication
Towards the Improved Discovery and Design of Functional Peptides: Common Features of Diverse Classes Permit Generalized Prediction of Bioactivity
Mooney C, Haslam NJ, Pollastri G, Shields DC (2012) Towards the Improved Discovery and Design of Functional Peptides: Common Features of Diverse Classes Permit Generalized Prediction of Bioactivity. PLOS ONE 7(10): e45012. https://doi.org/10.1371/journal.pone.0045012

## 5. Commercial Licensing

Commercial Use requires a separate written licence from the Licensor.
Enquiries: catherine.mooney@ucd.ie

## 6. No Warranty

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND. TO THE
MAXIMUM EXTENT PERMITTED BY LAW, THE LICENSOR SHALL NOT BE LIABLE FOR
ANY CLAIM, DAMAGES, OR OTHER LIABILITY ARISING FROM THE SOFTWARE OR ITS USE.

## 7. Termination

This Licence terminates automatically if you breach it. On termination,
you must stop using and destroy all copies of the Software in your
possession.

## 8. Governing Law

This Licence is governed by the laws of Ireland.

---

**PeptideRanker is a web server for predicting whether peptide sequences are likely to possess bioactivity.**
<p align="center">
    <img
      src="../frontpage/public/previews/peptideranker.png"
      alt="PeptideRanker"
      width="500"
    >
  </a>
</p>


## To set up:


In PeptideRanker.pl
set $pipeline_dir="."; to the full path to the PeptideRanker executable
(Models_Long.txt, Models_Short.txt and the Models folder need to be in the same location as the PeptideRanker executable) 


## To run:

To rank a list of peptides in order of predicted probability of being bioactive:

./PeptideRanker.pl input_file output_file
e.g. ./PeptideRanker.pl testfile.txt testfile.ranked
