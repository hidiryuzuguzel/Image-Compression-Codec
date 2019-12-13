#!/bin/bash
../icodec encode city.gray _city.enc 496 400
diff city.enc _city.enc 

../icodec decode _city.enc _citydec.gray 496 400
diff citydec.gray _citydec.gray 