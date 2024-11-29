#define DEF(e, str, s) e

enum stats_e{
#include "stats.conf"
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
#include <stdio.h>

const int do_element_id[ELEM_COUNT] = {DO_AIR, DO_EAU, DO_FEU, DO_TERRE, DO_NEUTRE};
const int stat_element_id[ELEM_COUNT]= {AGI, CHA, INT, FOR, FOR};

int base_damage_to_coeff(int element, int crit, double min_damage, double max_damage, double min_crit_damage, double max_crit_damage, double coeffs[]);
double compute_coeff(int crit, double min_damage, double max_damage, double min_crit_damage, double max_crit_damage);
int print_coeffs(double coeffs[]);

int main()
{
	int i;
	int crit;
	double min_damage, max_damage, min_crit_damage, max_crit_damage;
	double coeffs[STATS_COUNT];
	for(i=0;i<STATS_COUNT;i++)
	{
		coeffs[i]=0;
	}
	crit = 17;
	min_damage= 9;
	max_damage= 11;
	min_crit_damage= 12;
	max_crit_damage= 14;
	base_damage_to_coeff(TERRE,crit, min_damage, max_damage, min_crit_damage, max_crit_damage, coeffs);
	print_coeffs(coeffs);
	return 0;
}

int base_damage_to_coeff(int element, int crit, double min_damage, double max_damage, double min_crit_damage, double max_crit_damage, double coeffs[STATS_COUNT])
{
	double value;
	value=compute_coeff(crit, min_damage, max_damage, min_crit_damage, max_crit_damage);
	coeffs[stat_element_id[element]]+=value;
	coeffs[PUI]+=value;
	coeffs[do_element_id[element]]+=1.;
	coeffs[DO_CRIT]+=crit/100.;
	return 0;
}

double compute_coeff(int crit, double min_damage, double max_damage, double min_crit_damage, double max_crit_damage)
{
	return ((min_damage + max_damage)/2*(100-crit)/100 + (min_crit_damage + max_crit_damage)/2*crit/100);
}

int print_coeffs(double coeffs[])
{
	int i;
	for(i=0; i<STATS_COUNT;i++) printf("%f\n", coeffs[i]);
	return 0;
}