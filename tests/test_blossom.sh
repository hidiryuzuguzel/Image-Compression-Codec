#!/bin/bash
../icodec encodergb blossom.rgb _blossom.enc 256 256
diff blossom.enc _blossom.enc 

../icodec decodergb _blossom.enc _blossomdec.rgb 256 256
diff blossomdec.rgb _blossomdec.rgb