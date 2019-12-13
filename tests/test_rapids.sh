#!/bin/bash
../icodec encode rapids.gray _rapids.enc 256 256
diff rapids.enc _rapids.enc 

../icodec decode _rapids.enc _rapidsdec.gray 256 256
diff rapidsdec.gray _rapidsdec.gray 