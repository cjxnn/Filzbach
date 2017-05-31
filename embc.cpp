#include "preamble.h"
#if MODEL == 20

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>

#include "filzbach.h"
#include "repressilator.h"

using namespace std;

const int iterations = 100;
const int simIter = 50;

const int range = 10;
const double threshold = 0.8;

double gamma1_true = 0.3;
const double gamma2_true = 0.4;
const double gamma3_true = 0.5;

const double mu1_true = 0.2;
const double mu2_true = 0.4;
const double mu3_true = 0.6;

const double beta1_true = 1.4;
const double beta2_true = 1.5;
const double beta3_true = 1.6;

const double alpha1_true = 4;
const double alpha2_true = 3;
const double alpha3_true = 5;

const double theta1_true = 0.02;
const double theta2_true = 0.02;
const double theta3_true = 0.01;

double m1_ini, m2_ini, m3_ini, p1_ini, p2_ini, p3_ini;
double m1_start, m2_start, m3_start, p1_start, p2_start, p3_start;

#define PAUSE // uncomment to run without pauses
void pause() { PAUSE }

/************************************************************/


void fake_data(){
	/* use this function to create fake data to test your analysis, before you try real data */
	double m1_new, m1_old = m1_ini;
	double m2_new, m2_old = m2_ini;
	double m3_new, m3_old = m3_ini;
	double p1_new, p1_old = p1_ini;
	double p2_new, p2_old = p2_ini;
	double p3_new, p3_old = p3_ini;

	int tid = table_getID("mydata");
	if (tid != -1)
		table_delete(tid);

	/* create internal table to hold data */
	table_create("mydata");

	/* define table structure */
	table_addcolumn("mydata", "m1");
	table_addcolumn("mydata", "m2");
	table_addcolumn("mydata", "m3");
	table_addcolumn("mydata", "p1");
	table_addcolumn("mydata", "p2");
	table_addcolumn("mydata", "p3");

	table_writevalue("mydata", "m1", 0, m1_ini);
	table_writevalue("mydata", "m2", 0, m2_ini);
	table_writevalue("mydata", "m3", 0, m3_ini);
	table_writevalue("mydata", "p1", 0, p1_ini);
	table_writevalue("mydata", "p2", 0, p2_ini);
	table_writevalue("mydata", "p3", 0, p3_ini);

	/* create fake data */
	for (int i = 1; i <= iterations; i++)
	{
		/* compute values of a fake data row */
		m1_new = getm(gamma1_true, m1_old, alpha1_true, 2, p3_old, theta1_true);
		m2_new = getm(gamma2_true, m2_old, alpha2_true, 2, p1_old, theta2_true);
		m3_new = getm(gamma3_true, m3_old, alpha3_true, 2, p2_old, theta3_true);
		p1_new = getp(beta1_true, m1_old, mu1_true, p1_old);
		p2_new = getp(beta2_true, m2_old, mu2_true, p2_old);
		p3_new = getp(beta3_true, m3_old, mu3_true, p3_old);

		/* write these values to the data (as if reading real data) */
		table_writevalue("mydata", "m1", i, getError(m1_new));
		table_writevalue("mydata", "m2", i, getError(m2_new));
		table_writevalue("mydata", "m3", i, getError(m3_new));
		table_writevalue("mydata", "p1", i, getError(p1_new));
		table_writevalue("mydata", "p2", i, getError(p2_new));
		table_writevalue("mydata", "p3", i, getError(p3_new));

		m1_old = m1_new;
		m2_old = m2_new;
		m3_old = m3_new;
		p1_old = p1_new;
		p2_old = p2_new;
		p3_old = p3_new;
	}

	/* output data to use later on */
	//table_output("fakedata", "./workspace/repressilator_fakedata.txt");

	m1_start = m1_new;
	m2_start = m2_new;
	m3_start = m3_new;
	p1_start = p1_new;
	p2_start = p2_new;
	p3_start = p3_new;

	return;
}

double extractValue(const string& s){
	vector<std::string> tokens;
	istringstream iss(s);
	string token;
	while (std::getline(iss, token, '\t'))   // but we can specify a different one
		tokens.push_back(token);

	return stod(tokens[17]);
}

void setup_refine_parameters()
{
	double val;

	/* each line defines one new parameter for our model */
	val = random(0, range);
	// parameter_create(<param_name>,<lower_bound>,<upper_bound>,<initial_value>,<type>,<is_fixed?>,<is displayed>);
	parameter_create("gamma1", 0, range, val, 0, -1, 1);

	val = random(0, range);
	parameter_create("gamma2", 0, range, val, 0, -1, 1);

	val = random(0, range);
	parameter_create("gamma3", 0, range, val, 0, -1, 1);

	val = random(0, range);
	parameter_create("mu1", 0, range, val, 0, -1, 1);

	val = random(0, range);
	parameter_create("mu2", 0, range, val, 0, -1, 1);

	val = random(0, range);
	parameter_create("mu3", 0, range, val, 0, -1, 1);

	val = random(0, range);
	parameter_create("beta1", 0, range, val, 0, -1, 1);

	val = random(0, range);
	parameter_create("beta2", 0, range, val, 0, -1, 1);

	val = random(0, range);
	parameter_create("beta3", 0, range, val, 0, -1, 1);

	val = random(0, range);
	parameter_create("alpha1", 0, range, val, 0, -1, 1);

	val = random(0, range);
	parameter_create("alpha2", 0, range, val, 0, -1, 1);

	val = random(0, range);
	parameter_create("alpha3", 0, range, val, 0, -1, 1);

	val = random(0, range);
	parameter_create("theta1", 0, range, val, 0, -1, 1);

	val = random(0, range);
	parameter_create("theta2", 0, range, val, 0, -1, 1);

	val = random(0, range);
	parameter_create("theta3", 0, range, val, 0, -1, 1);

	val = random(0, range);
	parameter_create("f", 0, range, val, 0, -1, 1);

	return;
}

void setup_refine_PFandOLSparameters() {
	ifstream paramsFile("./workspace/PForOLS_MCMC_params_out.txt");
	string line;
	double val;

	getline(paramsFile, line);
	getline(paramsFile, line);
	val = extractValue(line);
	parameter_create("gamma1", threshold * val, (2 - threshold) * val, val, 0, -1, 1);

	getline(paramsFile, line);
	val = extractValue(line);
	parameter_create("gamma2", threshold * val, (2 - threshold) * val, val, 0, -1, 1);

	getline(paramsFile, line);
	val = extractValue(line);
	parameter_create("gamma3", threshold * val, (2 - threshold) * val, val, 0, -1, 1);

	getline(paramsFile, line);
	val = extractValue(line);
	parameter_create("mu1", threshold * val, (2 - threshold) * val, val, 0, -1, 1);

	getline(paramsFile, line);
	val = extractValue(line);
	parameter_create("mu2", threshold * val, (2 - threshold) * val, val, 0, -1, 1);

	getline(paramsFile, line);
	val = extractValue(line);
	parameter_create("mu3", threshold * val, (2 - threshold) * val, val, 0, -1, 1);

	getline(paramsFile, line);
	val = extractValue(line);
	parameter_create("beta1", threshold * val, (2 - threshold) * val, val, 0, -1, 1);

	getline(paramsFile, line);
	val = extractValue(line);
	parameter_create("beta2", threshold * val, (2 - threshold) * val, val, 0, -1, 1);

	getline(paramsFile, line);
	val = extractValue(line);
	parameter_create("beta3", threshold * val, (2 - threshold) * val, val, 0, -1, 1);

	getline(paramsFile, line);
	val = extractValue(line);
	parameter_create("alpha1", threshold * val, (2 - threshold) * val, val, 0, -1, 1);

	getline(paramsFile, line);
	val = extractValue(line);
	parameter_create("alpha2", threshold * val, (2 - threshold) * val, val, 0, -1, 1);

	getline(paramsFile, line);
	val = extractValue(line);
	parameter_create("alpha3", threshold * val, (2 - threshold) * val, val, 0, -1, 1);

	getline(paramsFile, line);
	val = extractValue(line);
	parameter_create("theta1", val > 0.1 ? val - 0.1 : 0, val + 0.1, val, 0, -1, 1);

	getline(paramsFile, line);
	val = extractValue(line);
	parameter_create("theta2", val > 0.1 ? val - 0.1 : 0, val + 0.1, val, 0, -1, 1);

	getline(paramsFile, line);
	val = extractValue(line);
	parameter_create("theta3", val > 0.1 ? val - 0.1 : 0, val + 0.1, val, 0, -1, 1);

	getline(paramsFile, line);
	val = extractValue(line);
	parameter_create("f", threshold * val, (2 - threshold) * val, val, 0, -1, 1);

	return;
}

double getm_with_f(const double gamma, const double m, const double alpha, const double n, const double p, const double theta, const double f, const double pp) {
	return m + delta_t * (alpha / (1 + pow(p, n)) - gamma * m + theta + f * pp);
}

void refine_OLSlikelihood()
{
	/* writes the log-likelihood given current parameter values */

	/* set sum over log-likelihood to zero */
	set_metr_ltotnew(0.0);
	set_metr_number_ok(0);

	double m1_new, m1_old = m1_ini;
	double m2_new, m2_old = m2_ini;
	double m3_new, m3_old = m3_ini;
	double p1_new, p1_old = p1_ini;
	double p2_new, p2_old = p2_ini;
	double p3_new, p3_old = p3_ini;

	/* read current parameter values */
	double gamma1 = cv("gamma1");
	double gamma2 = cv("gamma2");
	double gamma3 = cv("gamma3");

	double mu1 = cv("mu1");
	double mu2 = cv("mu2");
	double mu3 = cv("mu3");

	double beta1 = cv("beta1");
	double beta2 = cv("beta2");
	double beta3 = cv("beta3");

	double alpha1 = cv("alpha1");
	double alpha2 = cv("alpha2");
	double alpha3 = cv("alpha3");

	double theta1 = cv("theta1");
	double theta2 = cv("theta2");
	double theta3 = cv("theta3");

	double f = cv("f");

	/* loop over data */
	int numdata = table_numrows("mydata");
	for (int i = 1; i <= numdata; i++)
	{
		/* read data stored in a table */
		double m1 = table_getvalue("mydata", "m1", i);
		double m2 = table_getvalue("mydata", "m2", i);
		double m3 = table_getvalue("mydata", "m3", i);
		double p1 = table_getvalue("mydata", "p1", i);
		double p2 = table_getvalue("mydata", "p2", i);
		double p3 = table_getvalue("mydata", "p3", i);

		/* you may compute and write some "predictions" to compare the data with */
		m1_new = getm_with_f(gamma1, m1_old, alpha1, 2, p3_old, theta1, f, p2_old);
		m2_new = getm(gamma2, m2_old, alpha2, 2, p1_old, theta2);
		m3_new = getm(gamma3, m3_old, alpha3, 2, p2_old, theta3);
		p1_new = getp(beta1, m1_old, mu1, p1_old);
		p2_new = getp(beta2, m2_old, mu2, p2_old);
		p3_new = getp(beta3, m3_old, mu3, p3_old);
		// table_writevalue("mydata", <coulumn_name>, ii, <pred_var>

		/* now we have all we need to compute the likelihood of this data row */
		/* increment overall log likelihood and sample size */

		inc_metr_ltotnew(-(m1_new - m1) * (m1_new - m1));
		inc_metr_ltotnew(-(m2_new - m2) * (m2_new - m2));
		inc_metr_ltotnew(-(m3_new - m3) * (m3_new - m3));
		inc_metr_ltotnew(-(p1_new - p1) * (p1_new - p1));
		inc_metr_ltotnew(-(p2_new - p2) * (p2_new - p2));
		inc_metr_ltotnew(-(p3_new - p3) * (p3_new - p3));
		inc_metr_number_ok(6);

		m1_old = m1_new;
		m2_old = m2_new;
		m3_old = m3_new;
		p1_old = p1_new;
		p2_old = p2_new;
		p3_old = p3_new;
	}

	return;
}

void refine_PFandOLSlikelihood()
{
	/* writes the log-likelihood given current parameter values */

	/* set sum over log-likelihood to zero */
	set_metr_ltotnew(0.0);
	set_metr_number_ok(0);

	double m1_new, m1_old = m1_ini;
	double m2_new, m2_old = m2_ini;
	double m3_new, m3_old = m3_ini;
	double p1_new, p1_old = p1_ini;
	double p2_new, p2_old = p2_ini;
	double p3_new, p3_old = p3_ini;

	/* read current parameter values */
	double gamma1 = cv("gamma1");
	double gamma2 = cv("gamma2");
	double gamma3 = cv("gamma3");

	double mu1 = cv("mu1");
	double mu2 = cv("mu2");
	double mu3 = cv("mu3");

	double beta1 = cv("beta1");
	double beta2 = cv("beta2");
	double beta3 = cv("beta3");

	double alpha1 = cv("alpha1");
	double alpha2 = cv("alpha2");
	double alpha3 = cv("alpha3");

	double theta1 = cv("theta1");
	double theta2 = cv("theta2");
	double theta3 = cv("theta3");
	
	double f = cv("f");

	double prob;

	/* loop over data */
	int numdata = table_numrows("mydata");
	for (int i = 1; i <= numdata; i++)
	{
		/* read data stored in a table */
		double m1 = table_getvalue("mydata", "m1", i);
		double m2 = table_getvalue("mydata", "m2", i);
		double m3 = table_getvalue("mydata", "m3", i);
		double p1 = table_getvalue("mydata", "p1", i);
		double p2 = table_getvalue("mydata", "p2", i);
		double p3 = table_getvalue("mydata", "p3", i);

		/* you may compute and write some "predictions" to compare the data with */
		m1_new = getm_with_f(gamma1, m1_old, alpha1, 2, p3_old, theta1, f, p2_old);
		m2_new = getm(gamma2, m2_old, alpha2, 2, p2_old, theta2);
		m3_new = getm(gamma3, m3_old, alpha3, 2, p2_old, theta3);
		p1_new = getp(beta1, m1_old, mu1, p1_old);
		p2_new = getp(beta2, m2_old, mu2, p2_old);
		p3_new = getp(beta3, m3_old, mu3, p3_old);
		// table_writevalue("mydata", <coulumn_name>, ii, <pred_var>

		/* now we have all we need to compute the likelihood of this data row */
		/* increment overall log likelihood and sample size */

		prob = normal_density(m1, m1_new, variance);
		inc_metr_ltotnew(log(prob));

		prob = normal_density(m2, m2_new, variance);
		inc_metr_ltotnew(log(prob));

		prob = normal_density(m3, m3_new, variance);
		inc_metr_ltotnew(log(prob));

		prob = normal_density(p1, p1_new, variance);
		inc_metr_ltotnew(log(prob));

		prob = normal_density(p2, p2_new, variance);
		inc_metr_ltotnew(log(prob));

		prob = normal_density(p3, p3_new, variance);
		inc_metr_ltotnew(log(prob));

		inc_metr_number_ok(6);

		m1_old = m1_new;
		m2_old = m2_new;
		m3_old = m3_new;
		p1_old = p1_new;
		p2_old = p2_new;
		p3_old = p3_new;
	}

	return;
}

/* ************************************************* */
/* The control function that calls everything else.  */
/* ************************************************* */
int main()
{
	srand(time(NULL));

	m1_ini = random(0, 1);	m2_ini = random(0, 1);	m3_ini = random(0, 1);	p1_ini = random(0, 1);	p2_ini = random(0, 1);	p3_ini = random(0, 1);

	atexit(pause); // sets useful pause when debugging in Visual Studio
				   // set the likelihood function pointer
	pfn_likelihood = &refine_OLSlikelihood;

	initialize_filzbach();
	// analysis name becomes part of all file names written by Filzbach
	name_analysis("PForOLS");
	fake_data();
	setup_refine_parameters();
	set_chains(10);
	runmcmc(1000000, 100000, 100000, 100000);
	
	atexit(pause); // sets useful pause when debugging in Visual Studio
	// set the likelihood function pointer
	pfn_likelihood = &refine_PFandOLSlikelihood;

	initialize_filzbach();
	// analysis name becomes part of all file names written by Filzbach
	name_analysis("PFandOLS");
	setup_refine_PFandOLSparameters();
	set_chains(10);
	runmcmc(1000000, 100000, 100000, 100000);
}
#endif