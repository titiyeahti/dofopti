#define DEF(e, str, s) e
enum stats_e{
#include "configfiles/stats.conf"
};
#undef DEF

#define DEF(e, str, s) s
const char* const stats_codes[] = {
#include "configfiles/stats.conf"
};
#undef DEF

enum elem_{
	AIR,
	EAU,
	FEU,
	TERRE,
	NEUTRE,
	ELEM_COUNT,
};

enum damage_computing_type_{
	MIN,
	MAX,
	AVERAGE,
};

#include <stdio.h>

const int do_element_id[ELEM_COUNT] = {DO_AIR, DO_EAU, DO_FEU, DO_TERRE, DO_NEUTRE};
const int stat_element_id[ELEM_COUNT]= {AGI, CHA, INT, FOR, FOR};

int base_damage_to_coeff(int element, int crit, int min_damage, int max_damage, int min_crit_damage, int max_crit_damage, double coeffs[], int nb_cases_pushback, int damage_computing_type);
double compute_coeff(int crit, int min_damage, int max_damage, int min_crit_damage, int max_crit_damage, int damage_computing_type);
int print_coeffs(double coeffs[]);
double compute_docri(int crit, int damage_computing_type);
double compute_pushback_damage_coeff(int nb_cases);

int main()
{
	int i;
	int crit;
	int element;
	int damage_computing_type;
	int min_damage, max_damage, min_crit_damage, max_crit_damage, nb_cases_pushback;
	double coeffs[STATS_COUNT];
	for(i=0;i<STATS_COUNT;i++)
	{
		coeffs[i]=0;
	}
	element= TERRE;
	crit = 17;
	min_damage= 9;
	max_damage= 11;
	min_crit_damage= 12;
	max_crit_damage= 14;
	nb_cases_pushback= 3;
	damage_computing_type= MIN;
	base_damage_to_coeff(element,crit, min_damage, max_damage, min_crit_damage, max_crit_damage, coeffs, nb_cases_pushback, damage_computing_type);
	print_coeffs(coeffs);
	return 0;
}

int base_damage_to_coeff(int element, int crit, int min_damage, int max_damage, int min_crit_damage, int max_crit_damage, double coeffs[STATS_COUNT], int nb_cases_pushback, int damage_computing_type)
{
	double value;
	value=compute_coeff(crit, min_damage, max_damage, min_crit_damage, max_crit_damage, damage_computing_type);
	coeffs[stat_element_id[element]]+=value;
	coeffs[PUI]+=value;
	coeffs[do_element_id[element]]+=1.;
	coeffs[DO_CRIT]+=compute_docri(crit, damage_computing_type);
	coeffs[DO_POU]+=compute_pushback_damage_coeff(nb_cases_pushback);
	return 0;
}

double compute_coeff(int crit, int min_damage, int max_damage, int min_crit_damage, int max_crit_damage, int damage_computing_type)
{
	double value=0;
	switch(damage_computing_type){
	case MIN:
		if (crit>= 100) value =min_crit_damage;
		else value=min_damage;
		break;
	case MAX:
		if (crit> 0) value =max_crit_damage;
		else value=max_damage;
		break;
	case AVERAGE:
		if (crit>= 0) value=(min_damage + max_damage)/2*(100-crit)/100 + (min_crit_damage + max_crit_damage)/2*crit/100;
		else value=(min_damage + max_damage)/2;
		break;
	default:
		value=0;
		break;
	}
	return value;
}

int print_coeffs(double coeffs[])
{
	int i;
	for(i=0; i<STATS_COUNT;i++)
	{
		if(coeffs[i]!=0.) printf("%s %f\n", stats_codes[i], coeffs[i]);
	}
	return 0;
}

double compute_docri(int crit, int damage_computing_type)
{
	double value=0;
	switch(damage_computing_type){
	case MIN:
		if (crit>= 100) value =1.;
		else value=0;
		break;
	case MAX:
		if (crit> 0) value =1.;
		else value=0;
		break;
	case AVERAGE:
		if (crit>= 0) value=crit/100.;
		else value=0;
		break;
	default:
		value=0;
		break;
	}
	return value;
}

double compute_pushback_damage_coeff(int nb_cases_pushback)
{
	return nb_cases_pushback/4.;
}