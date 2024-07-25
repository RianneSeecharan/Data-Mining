#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::fstream;
using std::ios;
using std::stringstream;


// Function which reads the csv file
vector<vector<string>> scrubCSV(string filename) {
    fstream dataFile;
    dataFile.open(filename, ios::in); //read mode
    //make sure is open
    vector<vector<string>>raw_data;
    if (dataFile.is_open()) {

        //scrub data
        //take out rows with happiness_index < 0 and green_space_data > 100

        //skip header 
        string header;
        getline(dataFile, header);

        string line;
        while (getline(dataFile, line)) {
            stringstream ss(line);
            vector<string> row;
            string field;
            int columnCount = 0;
            bool skipRow = false; //marker 

            while (getline(ss, field, ',')) {
                if ((columnCount == 7 && stoi(field) < 0) || (columnCount == 5 && stoi(field) > 100)) {
                    skipRow = true; //if this occurs, this row has an error
                    break; 
                }
                row.push_back(field); 
                columnCount++;
            }

            // Only adds the row if it should not be skipped
            if (!skipRow) {
                raw_data.push_back(row);
            }
        

    }
    dataFile.close();
}
    else {
        cout << "Error opening file." << endl; //error message if file cannot open
    }
    return raw_data;
    }

// Separate columns into different vectors
vector<vector<string>> separateColumns(const vector<vector<string>>& matrix) {
    int cols = matrix[0].size(); // Number of columns
    vector<vector<string>> separatedColumns(cols); // Vector to store separated columns

    for (const auto& row : matrix) {
        for (int i = 0; i < cols; ++i) {
            separatedColumns[i].push_back(row[i]); // Push each element into the corresponding column vector
        }
    }

    return separatedColumns;
}

//convert column vector value from string to int
vector<int>stringToInt(vector<string> x) {
    vector<int> converted;
    int convertValue;
    for (int i = 0; i < x.size(); i++) {
        stringstream iss(x[i]);
        if (iss >> convertValue) {
            converted.push_back(convertValue);
        }
    }
    return converted;
}

//convert column vector value from string to double (Happiness Score)
vector<double>stringToDouble(vector<string> x) {
    vector<double> converted;
    double convertValue;
    for (int i = 0; i < x.size(); i++) {
        stringstream iss(x[i]);
        if (iss >> convertValue) {
            converted.push_back(convertValue);
        }
    }
    return converted;
}
//normalize data set 
vector<double> normalize_int_vector(vector<int> x) {
    //find maximum
    double max = 0;
    for (int i = 0; i < x.size(); i++) {
        if (x[i] > max) {
            max = x[i];
        }
    }
    //find minumum
    double min = max;
    for (int i = 0; i < x.size(); i++) {
        if (x[i] < min) {
            min = x[i];
        }
    }
    //find range
    double range = max - min;
    //xnormalized = (x - xminimum) / range of x
    vector<double> norm_vector;
    double norm_value;
    for (int i = 0; i < x.size(); i++) {
        norm_value = (x[i] - min) / range;
        norm_vector.push_back(norm_value);
    }

    return norm_vector; 
}

//do the same for happiness score (diff type of vector)

vector<double> normalize_HS_vector(vector<double> x) {
    //find maximum
    double max = 0;
    for (int i = 0; i < x.size(); i++) {
        if (x[i] > max) {
            max = x[i];
        }
    }
    //find minumum
    double min = max;
    for (int i = 0; i < x.size(); i++) {
        if (x[i] < min) {
            min = x[i];
        }
    }
    //find range
    double range = max - min;
    //xnormalized = (x - xminimum) / range of x
    vector<double> norm_vector;
    double norm_value;
    for (int i = 0; i < x.size(); i++) {
        norm_value = (x[i] - min) / range;
        norm_vector.push_back(norm_value);
    }

    return norm_vector;
} 

double average(vector<double> x) {
    double sum = 0;
    for (int i = 0; i < x.size(); i++) {
        sum = sum + x[i];
    }
    double avg = sum / x.size(); 
    return avg; 
}




//find Sxx 
double Sxx(vector<double> x) {
    //find xavg
    double xavg = average(x);  

    //Find Sxx
    double sumOfSquares = 0; 
    for (int i = 0; i < x.size(); i++) {
        sumOfSquares = sumOfSquares + pow((x[i] - xavg), 2); 
    }
    double Sxx = sumOfSquares / x.size();
    return Sxx;
}

// Calculate Sxy
double Sxy(vector<double> y, vector<double> x) {
    
    // Calculate the means of x and y
    double yavg = average(y);  
    double xavg = average(x); 

    // Calculate the sum of the products of differences
    double sumOfProducts = 0;
    for (int i = 0; i < y.size(); i++) {
        sumOfProducts += (y[i] - yavg) * (x[i] - xavg);
    }

    // Calculate Sxy
    double Sxy = sumOfProducts / y.size();

    return Sxy; 
}
//find regression coefficient
double regressionCoeff(double Sxx, double Sxy) {
    double b = Sxy / Sxx;
    return b; 
}

//get equation of line given b, xavg, yavg (find c)
//y=bx+c

double yintercept(double b, double xavg, double yavg) {
    double c = yavg - (b * xavg);
    cout << "eqn of regression line: y=" << b << "x" << "+ (" << c << ")" << endl;
    return c;
}

//use equation to make y points for x vector and plug into a csv file
vector<double> regressionPoints(vector<double> x, double b, double c) {
    //y= mx + c
    vector<double> y;
    double yval;
    for (int i = 0; i < x.size(); i++) {
        yval = (b * x[i]) + c;
        y.push_back(yval);
    }
    return y; 
}

//write x value and corresponding y value into csv file 
void resulltCSV(string filename, vector<double> x, vector<double>y, string xcolumn){
        fstream resultFile;
        resultFile.open(filename, ios::out);
        if (resultFile.is_open()) { 
            resultFile << xcolumn << "," << "HS" << endl;  
            for (int i = 0; i < x.size(); i++) { 
                resultFile << x[i] << " , " << y[i] << endl;
             }
            cout << filename << " result csv made successfully." << endl; 
            resultFile.close(); 
        }

        else {
            cout << "ERROR opening resultfile" << filename<< endl; 
        }
    } 

//get regression equations and result csv files for objectives
void yVSx(vector<double>y, vector<double>x, string xname, string filename) {

    double SxxVal = Sxx(x);
    double SxyVal = Sxy(y, x);
    double b = regressionCoeff(SxxVal, SxyVal);

    if (b > 0) {
        cout << "Regression coefficient is positive (" << b << ") thus Happiness Score increases as " <<  xname  << "increases" << endl;
    }
    else {
        cout << "Regression coefficient is negative (" << b << ") thus Happiness Score decreases as "<< xname<< " increases" << endl;
    }
    double xavg = average(x);
    double yavg = average(y);
    double c= yintercept(b, xavg, yavg); 

    vector<double> yreg= regressionPoints(x, b, c); //get regression y points for x points 
    resulltCSV(filename, x, yreg, xname);   //make csv to graph regression line   

}


int main() {
    //open file with train or test
    vector<vector<string>> scrubdata = scrubCSV("test.csv"); //change to suit 
    // Separate columns into different vectors
    vector<vector<string>> separatedColumns = separateColumns(scrubdata);

    //names of used columns
    vector<string> D_L, T_D, G_S_A, A_Q_I, H_S, C_L_I, H_I;
    D_L = separatedColumns[3];
    T_D = separatedColumns[4];
    G_S_A = separatedColumns[5];
    A_Q_I = separatedColumns[6];
    H_S = separatedColumns[7];
    C_L_I = separatedColumns[8];
    H_I = separatedColumns[9];
    //typecast
    //function to convert string vector to int vector for D_L, G_S_A, A_Q_I, C_L_I, H_I. 
    vector<int> convD_L = stringToInt(D_L);
    vector<int> convG_S_A = stringToInt(G_S_A); 
    vector<int> convA_Q_I = stringToInt(A_Q_I); 
    vector<int> convC_L_I = stringToInt(C_L_I); 
    vector<int> convH_I = stringToInt(H_I);
 
 //normalize int vectors (will turn into double)
    vector<double> normD_L = normalize_int_vector(convD_L);
    vector<double> normG_S_A = normalize_int_vector(convG_S_A);  
    vector<double> normA_Q_I = normalize_int_vector(convA_Q_I);  
    vector<double> normC_L_I = normalize_int_vector(convC_L_I); 
    vector<double> normH_I = normalize_int_vector(convH_I); 


    //function to convert string vector to double vector for H_S, and then normalize
    vector<double>convH_S = stringToDouble(H_S);
    vector<double> normH_S = normalize_HS_vector(convH_S);     


    //regression result for H_S vs D_L
    yVSx(normH_S, normD_L,"DL", "HSvsDL.csv");  

    //regression result for H_S vs G_S_A
    yVSx(normH_S, normG_S_A, "GSA", "HSvsGSA.csv");

    //regression result for H_S vs A_Q_I
    yVSx(normH_S, normA_Q_I, "AQI", "HSvsAQI.csv");

    //regression result for H_S vs C_L_I
    yVSx(normH_S, normC_L_I, "CLI", "HSvsCLI.csv");

    //regression result for H_S vs H_I
    yVSx(normH_S, normH_I, "HI", "HSvsHI.csv");


	return 0;
}

