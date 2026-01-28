*KEYWORD
*TITLE
Simple Shell Model
$---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8
$ Created by KooSimulationDataReader test
$
*NODE
         1       0.0       0.0       0.0
         2     100.0       0.0       0.0
         3     100.0     100.0       0.0
         4       0.0     100.0       0.0
         5      50.0      50.0       0.0
*ELEMENT_SHELL
         1         1         1         2         5         0
         2         1         2         3         5         0
         3         1         3         4         5         0
         4         1         4         1         5         0
*PART
Shell Part
         1         1         1         0         0         0         0         0
*SECTION_SHELL
         1         2       1.0         2       0.0         0         0       1.0
       1.5       1.5       1.5       1.5       0.0
*MAT_ELASTIC
         1   7.85-9    2.1+5       0.3       0.0       0.0       0.0
*END
