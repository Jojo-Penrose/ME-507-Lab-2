/*! @file AERO-553-Bonus.c
    @brief AERO 553 bonus project.
    @author Jojo Penrose
    @date Spring 2024
    
    This file contains the implementation and demonstration code for the AERO
    553 bonus project. The assignment was to implement the exponent operator,
    X^Y, for all real numbers in a compiled language (C, in this case).
    
    While the domain of X^Y is all real numbers, the range includes both real
    and non-real results. As such, the implementation is fairly complex and is
    documented separately.
    
    Implicitly, the restrictions on computation are limited to 64-bit numbers
    and multiplication, division, addition, subtraction, modulo, and comparison.
    
    Helper functions were written in order to implement, in order:
        - Integer exponentiation (WholeExp)
        - Integer factorials (IntFactorial)
        - Rationalization and simplification of decimal numbers (Rationalize)
        - Exponential function using the power series expansion (NatExp)
        - Natural logarithm using Halley's method (HalleyLN)
        - First root of -1 using de Moivre's Formula (NonRealRoot)
        - Cosine function using the Maclaurin series (Cosine)
        - Sine function using the Maclaurin series (Sine)
        
    The function BonusExp uses all of these helper functions to calculate X^Y.
    The only important note as to the results of BonusExp is that, for operands
    that produce potentially non-real results, the way that the (-1) root is
    handled in NonRealRoot pulls the first root by de Moivre's definition; that
    is, (-1) has n nth roots, and NonRealRoot may choose a root that does not
    appear to agree with other calculators. WolframAlpha in the web may be used 
    to see all possible results for such operands.
*/
#include <stdio.h>

void BonusExp(double X, double Y, double *result);              // CHECK
double WholeExp(double Op, int Exp);                            // CHECK
unsigned long long int IntFactorial(unsigned long long int F);  // CHECK
void Rationalize(double X, unsigned long long int *result);     // CHECK
double NatExp(double X);                                        // CHECK
double HalleyLN(double X);                                      // CHECK
void NonRealRoot(unsigned long long int X, double *result);     // CHECK  
double Cosine(double X);                                        // CHECK
double Sine(double X);                                          // CHECK                                   

/*! @brief Main function.
    
    Prompts the user to enter a base and exponent, then reports the result.
*/
int main()
{
    double x;
    double y;
    double result[2];
	
	printf("This is the demonstration program for the AERO 553 Bonus Project.\r\n");
	printf("Written by Jojo Penrose, SP 2024 at Cal Poly.\r\n\n");
	
	printf("This script will calculate X^Y for all real numbers.\r\n\n");
	
	printf("To begin, enter a base X.\r\n");
	scanf("%lf", &x);
	
	printf("\nEnter an exponent Y.\r\n");
	scanf("%lf", &y);
	
    BonusExp(x, y, result);
    
    if (result[1] == 0) {
    printf("\n(%g)^(%g) = %g", x, y, result[0]);
    } else {
    printf("\n(%g)^(%g) = (%g) + i(%g)", x, y, result[0], result[1]);
    }
    
    return 0;
}



/*! @brief Real exponentiation function.
    
    Computes X^Y for all real numbers. Reports a complex result by default.
    
    @param  X       Base
    @param  Y       Exponent 
    @param  result  >2-element result buffer for the real and non-real parts
    @return         None
*/
void BonusExp(double X, double Y, double *result)
{
    // Catch special cases
    if (X == 0) {
        // 0^y = 0
        result[0] = 0;
        result[1] = 0;
        
        return;
    } else if (Y == 0) {
        // x^0 = 1
        result[0] = 1;
        result[1] = 0;
        
        return;
    } else if (X == 1) {
        // 1^y = 1
        result[0] = 1;
        result[1] = 0;
        
        return;
    } else if (Y == 1) {
        // x^1 = x
        result[0] = X;
        result[1] = 0;
        
        return;
    }
    
    // Make variables
    double T1[2];                       // (-1) term
    double T2;                          // real numerator term
    double T3;                          // real denominator term
    unsigned long long int a;           // base numerator
    unsigned long long int b;           // base denominator
    unsigned long long int c;           // exponent numerator
    unsigned long long int d;           // exponent denominator
    unsigned long long int ratbuf[2];   // ratonalizer buffer
    double rootbuf[2];                  // T1 root buffer
    int m;                              // base sign flag
    int n;                              // exponent sign flag
    
    // Rationalize the base
    if (X < 0) {
        // If X be negative...
        m = 1;
        X = -X;
        // ...set sign flag negative and flip
    } else {
        // Elsewise, set sign flag positive
        m = 0;
    }
    Rationalize(X, ratbuf); // Send X to be rationalized
    a = ratbuf[0];
    b = ratbuf[1];          // Store X num and den

    // Rationalize the power
    if (Y < 0) {
        // If Y be negative...
        n = 1;
        Y = -Y;
        // ...set sign flag negative and flip
    } else {
        // Elsewise, set sign flag positive
        n = 0;
    }
    Rationalize(Y, ratbuf);         // Send Y to be rationalized
    c = ratbuf[0];
    d = ratbuf[1];                  // Store Y num and den

    // Calculate T1
    if (m == 0) {
        // If X is positive...
        T1[0] = 1;
        T1[1] = 0;
        // ...T1 is just 1
    } else if (c % 2 == 0) {
        // If X is negative, but the sign flips by even power...
        T1[0] = 1;
        T1[1] = 0;
        // ...T1 is just 1
    } else {
        // X may have non-real roots
        NonRealRoot(d, rootbuf);    // Calculate first complex root
        T1[0] = rootbuf[0];
        T1[1] = rootbuf[1];         // Store results in T1
    }
    
    // Calculate T2
    T2 = NatExp((double)c/(double)d * HalleyLN(a)); // Calculate the exponent
    if (n == 1) {
        // If the exponent was negative...
        T2 = 1/T2;
        // ...T2 needs to flip
    }
    
    // Calculate T3
    T3 = NatExp((double)c/(double)d * HalleyLN(b)); // Calculate the exponent
    if (n == 0) {
        // If the exponent was positive...
        T3 = 1/T3;
        // ...T3 needs to flip
    }
    
    // Combine terms and return results
    result[0] = T1[0]*T2*T3;
    result[1] = T1[1]*T2*T3;

    return;
}



/*! @brief Integer exponentiation function.
    
    Recursive function that computes X^n for integer powers.
    
    @param  X       Base
    @param  n       Integer exponent 
    @return         X^n
*/
double WholeExp(double X, int n)
{
    if (n == 0) {
        return 1;
    } else if(n > 0) {
        return X*WholeExp(X, n-1);
    } else {
        return 1/(WholeExp(X, -n));
    }
}



/*! @brief Integer factorial function.
    
    Recursive function that computes F! for positive integers. Because it is
    not required for BonusExp, the negative factorials are not properly
    implemented.
    
    @param  F       Operand
    @return         F!
    @note           64-bit computers are limited to 20!
*/
unsigned long long int IntFactorial(unsigned long long int F)
{
    if(F <= 1) {
        return 1;
    } else {
        return F*IntFactorial(F-1);
    }
}



/*! @brief Function to get a simplified fraction from a real, positive decimal.
    
    The operation relies on the fact that computers cannot store irrational
    numbers. Therefore, any real computer-realizable number can be described by
    a ratio of two integers, most simply by a denominator that is a power of
    10. For example:
    
                       314159
            3.14159 = --------
                       100000
                       
    After converting the number to a ratio using a power of ten, the function
    uses a list of the first 26 prime numbers to factor the ratio as much as
    possible. Finally, the simplified numerator and denominator are stored in
    the result buffer.
    
    Because it is not required for BonusExp, negative inputs are not fully
    tested.
    
    @param  X       Input real, positive number
    @param  result  Buffer to store the simplified numerator and denominator in
    @return         None
*/
void Rationalize(double X, unsigned long long int *result)
{
    unsigned long long int a = (unsigned long long int)X;   // numerator
    unsigned long long int b;                               // denominator
    double bdub = 0.1;                                      // working b
    //printf("%lld\r\n", a);
    int safe = 0;                                           // iteration count
    // List of the first 26 prime numbers:
    static int primes[26]= {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43,
                            47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101};
    
    // Determine the smallest acceptable power of ten denominator
    // Keep checking if rounded off X*10^n equals a
    while (X*bdub - a != 0) {
        // Get next power of ten
        bdub = bdub*10;
        // Increment iteration count
        safe++;
        // Set a to X times the new power of ten
        a = (unsigned long long int)(X*bdub);
        
        // If b goes too high, break out and report error 
        // Most likely, an error has occurred and the digit limit was exceeded
        if (safe > 20) {
            printf("ERROR: RATIONALIZATION LIMIT EXCEEDED. DISCARD RESULT");
            break;
        }
    }
    
    // Store the accepted b value
    b = (unsigned long long int)bdub;
    
    //printf("%f = %lld / %lld\r\n", X, a, b);
    
    // Factor a/b by sequentially dividing by prime factors
    // Iterate over the first 26 primes
    for (int p = 0; p < 25; p++) {
        while (a%primes[p] == 0 && b%primes[p] == 0) {
            // If the prime divides evenly into a and b...
            a = a/primes[p];
            b = b/primes[p];
            // ...divide a and b by the prime
        }
    }
    
    //printf("%f = %lld / %lld\r\n", X, a, b);
    
    // Store the results and return
    result[0] = a;
    result[1] = b;
    
    return;
}



/*! @brief Function to calculate e^X using the power series expansion.
    
    The operation uses the power series expansion to calculate e^X. It will
    continue to sum up sequential power series terms until it successfully
    detects convergence within six decimal points.
    
    The series does not converge well for X > 1, and doesn't converge at all
    for X > 3 (within a number of iterations that this machine can compute).
    To get around this, the function takes advantage of power rules that say:
    
                e^(X) = e^(X * N/N) = ( e^(X/N) )^(N)
                
    By limiting N to be positive integers, the power series can be used to
    calculate e^(X/N) and that result can be then raised to the Nth power using
    WholeExp, which effectively guarantees quick convergence to e^X.
    
    The largest reason for this limitation is that the series uses factorials,
    and 64-bit computers are limited to 20!, and thus, 20 terms of the power
    series.
    
    Because it is not required for BonusExp, negative inputs are not fully
    tested.
    
    @param  X       Input real number
    @return         e^X
*/
double NatExp(double X)
{
    double sum = 0;                 // Current power series sum
    double sumold;                  // Power series sum at last term
    int N = 1;                      // Integer scaling power
    double XoverN = X/(double)N;    // Scaled power for e^(X/N)
    
    // Continue reducing X/N until it is < 2 by incrementing N continuously
    while (XoverN > 2) {
        N++;
        XoverN = X/(double)N;
    }
    
    // Execute the power series expansion
    for (int k = 0; k < 21; k++) {
        // Record last summation
        sumold = sum;
        
        // Compute next term in the power series
        sum += WholeExp(XoverN, k)/(double)IntFactorial(k);
        
        // Check for convergence within six decimal points
        if (sum - sumold < 0.000001 && sum - sumold > -0.000001) {
            //printf("NatExp converged in %d iterations\r\n", k);
            break;
        } else if (k == 20) {
            // Alert user if NatExp fails to converge, and with what error
            printf("WARNING: NatExp iterations exceeded with %f error\r\n", sum - sumold);
            break;
        }
    }
    
    // Raise e^(X/N) to the scaling power N and return the result
    return WholeExp(sum, N);
}



/*! @brief Function to calculate the natural logarithm using Halley's method.
    
    Halley's method is a modified form of Newton's method for numerically
    calculating the natural logarithm. It can be derived from Newton's method
    by leveraging the fact that the derivative of the exponential is itself.
    
    Halley's method requires an initial guess value, and it tends to converge
    significantly faster if the guess value is higher than the correct result.
    One advantageous fact of the natural logarithm is that the limit of ln is
    infinite, but the limit of its slope approaches zero at infinity. This means
    that the slope of the natural logarithm becomes insignificant with respect
    to its input when the input is large enough. The largest possible 64-bit
    unsigned integer is 18446744073709551615, and:
    
            ln(18446744073709551615) = 44.3614195558

    Using 45 as an initial guess is sufficient to achieve convergence for all 
    possible 64-bit numbers within the domain of the natural logarithm (X > 0).
    
    @param  X       Input real number > 0
    @return         ln(X)
    @warning        X is required to be > 0 !
*/
double HalleyLN(double X)
{
    double Yold;        // Last guess value
    double Ynew = 45;   // Initial: the largest 64-bit natural logarithm is 44.4

    for (int k = 0; k < 10001; k++) {
        // Store last guess
        Yold = Ynew;
        
        // Compute next guess
        Ynew = Yold + 2*(X - NatExp(Yold))/(X + NatExp(Yold));
        
        // Check for convergence within six decimal points
        if (Yold - Ynew < 0.000001 && Ynew - Yold > -0.000001) {
            //printf("HalleyLN converged in %d iterations\r\n", k);
            break;
        } else if (k == 10000) {
            // Alert user if HalleyLN fails to converge, and with what error
            printf("WARNING: HalleyLN iterations exceeded with %f error\r\n", Yold - Ynew);
            break;
        }
    }
    
    // Return result
    return Ynew;
}



/*! @brief Function to calculate (-1)^(1/X).
    
    Uses de Moivre's Formula to compute the first Xth root of (-1). An imaginary
    number as X Xth roots, but this function only pulls the first. Applications
    like WolframAlpha are programmed to return all X Xth roots, and other
    calculators might use other roots in their algorithms. The first Xth root 
    is simply the easiest to calculate.
    
    Because it is not required for BonusExp, negative inputs are not fully
    tested.
    
    @param  X       Positive integer root
    @param  result  >2-element result buffer for the real and non-real parts
    @return         None
*/
void NonRealRoot(unsigned long long int X, double *result)
{
    static double pi = 3.14159265359;       // pi!
    
    // Calculate and store results, then return
    result[0] = Cosine(pi/X);               // Real part
    result[1] = Sine(pi/X);                 // Non-real part
    
    return;
}



/*! @brief Function to cosine in radians.
    
    Uses the first ten terms of the Maclaurin series to estimate the cosine 
    function. The series is limited to ten terms because 64-bit computers
    cannot compute factorials greater than 20!.
    
    The series does not converge quickly for inputs with magnitude > 2pi. Since
    the trigonometric functions periodic, the function intentionally removes
    full rotations from the input in order to guarantee convergence within the
    computational limit.
    
    Because it is not required for BonusExp, negative inputs are not fully
    tested.
    
    @param  X       Input in radians
    @return         cos(X)
*/
double Cosine(double X)
{
    static double tpi = 2*3.14159265359;    // 2pi!
    double sum = 0;                         // Current series summation
    double sumold;                          // Summation at the previous term

    double Xlt2pi = X;                       // X with full rotations removed
    
    // Subtract rotations from postive radians untill X is less than 2pi:
    while (Xlt2pi > tpi) {
        Xlt2pi -= tpi;
    }
    
    // Add rotations to negative radians untill X is greater than -2pi:
    while (Xlt2pi < -tpi) {
        Xlt2pi += tpi;
    }
    
    // Compute the Maclaurin series summation
    for (int k = 0; k < 11; k++) {
        // Store last summation
        sumold = sum;
        
        // The series is alternating
        if (k%2 == 0) {
            // Positive (even) terms of the series
            sum += WholeExp(Xlt2pi, 2*k) / IntFactorial(2*k);
        } else {
            // Negative (odd) terms of the series
            sum -= WholeExp(Xlt2pi, 2*k) / IntFactorial(2*k);
        }
        
        // Check for series convergence
        if (sum - sumold < 0.000001 && sum - sumold > -0.000001) {
            //printf("Cosine converged in %d iterations\r\n", k);
            break;
        } else if (k == 10) {
            // Alert user if Cosine fails to converge, and with what error
            printf("WARNING: Cosine iterations exceeded with %f error\r\n", sum - sumold);
            break;
        }
    }
    
    // Return the result
    return sum;
}



/*! @brief Function to sine in radians.
    
    Uses the first nine terms of the Maclaurin series to estimate the sine 
    function. The series is limited to ten terms because 64-bit computers
    cannot compute factorials greater than 20!.
    
    The series does not converge quickly for inputs with magnitude > 2pi. Since
    the trigonometric functions periodic, the function intentionally removes
    full rotations from the input in order to guarantee convergence within the
    computational limit.
    
    Because it is not required for BonusExp, negative inputs are not fully
    tested.
    
    @param  X       Input in radians
    @return         sin(X)
*/
double Sine(double X)
{
    static double tpi = 2*3.14159265359;    // 2pi!
    double sum = 0;                         // Current series summation
    double sumold;                          // Summation at the previous term

    double Xlt2pi = X;                       // X with full rotations removed
    
    // Subtract rotations from postive radians untill X is less than 2pi:
    while (Xlt2pi > tpi) {
        Xlt2pi -= tpi;
    }
    
    // Add rotations to negative radians untill X is greater than -2pi:
    while (Xlt2pi < -tpi) {
        Xlt2pi += tpi;
    }
    
    // Compute the Maclaurin series summation
    for (int k = 0; k < 10; k++) {
        // Store last summation
        sumold = sum;
        
        // The series is alternating
        if (k%2 == 0) {
            // Positive (even) terms of the series
            sum += WholeExp(Xlt2pi, 2*k+1) / IntFactorial(2*k+1);
        } else {
            // Negative (odd) terms of the series
            sum -= WholeExp(Xlt2pi, 2*k+1) / IntFactorial(2*k+1);
        }
        
        // Check for series convergence
        if (sum - sumold < 0.000001 && sum - sumold > -0.000001) {
            //printf("Sine converged in %d iterations\r\n", k);
            break;
        } else if (k == 9) {
            // Alert user if Cosine fails to converge, and with what error
            printf("WARNING: Sine iterations exceeded with %f error\r\n", sum - sumold);
            break;
        }
    }
    
    // Return the result
    return sum;
}