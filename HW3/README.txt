Hi this is Ziwen, and this is the README of HW3 of AMATH483


Use following code to build shared Object Library:
cd HW3
g++ -std=c++17 -fPIC -shared q1-3/ref_daxpy.cpp q1-3/ref_dgemv.cpp q1-3/ref_dgemm.cpp q4-5/ref_axpyt.cpp q4-5/ref_gemvt.cpp q4-5/ref_gemmt.cpp -o librefBLAS.so



Compile the testHW3.cpp with following command to specify using the .so you just created:
g++ -std=c++17 testHW3.cpp -L. -lrefBLAS -o test_exec