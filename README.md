Error is inherent to floating point arithmetic due to its approximation of real values with finite precision. In some cases, compounded results of slightly erroneous floating point operations can lead to amounts of error that are unacceptable for applications requiring consistent accuracy (like banking). The aim of this thesis is to address unacceptable errors in black box floating point functions by recognizing and patching these errors. After error recognition, we use non-erroneous function regions to predict relationships between different parts of the function. These relationships (or properties) can then be used to patch the function. Current findings indicate strong possibility of localizing black box floating point function error with the help of a user oracle and demonstrate clear capabilities of patching error after property generation. Future research directions include improving interfacing between the stages of this process and strengthening error localization capabilities.