/*
 * TransmissionRunner.cpp
 *
 *  Created on: Jan 19, 2016
 *      Author: nick
 */

#include "TransmissionRunner.h"

namespace TransModel {

TransmissionRunner::TransmissionRunner(float circumcision_multiplier, float prep_multiplier, std::vector<float>& given_dur_inf_by_age) :
		circumcision_multiplier_(circumcision_multiplier), prep_multiplier_(prep_multiplier),
		dur_inf_by_age(given_dur_inf_by_age) {
}

TransmissionRunner::~TransmissionRunner() {
}

bool TransmissionRunner::determineInfection(PersonPtr infector, PersonPtr infectee) {
	float infectivity = infector->infectivity();

	// modify infectivity appropriately
	if (infectee->onPrep()) {
		infectivity *= prep_multiplier_;
	}

	/*
	// TODO: need to figure how to handle insertive / receptive and
	// switching them
	if (infector is receptive && infectee is circumcised) {
		infectivity *= circumcision_multiplier_;
	}
	*/
	return infectivity >= repast::Random::instance()->nextDouble();
}

void TransmissionRunner::infect(PersonPtr infectee) {
	int draw = (int) repast::Random::instance()->getGenerator(ART_COVERAGE_BINOMIAL)->next();
	float dur_inf = 0;
	if (infectee->age() <= 24) {
		dur_inf = dur_inf_by_age[0];
	} else if (infectee->age() <= 34) {
		dur_inf = dur_inf_by_age[1];
	} else if (infectee->age() <= 44) {
		dur_inf = dur_inf_by_age[2];
	} else {
		dur_inf = dur_inf_by_age[3];
	}

	infectee->infect(draw == 1, dur_inf);
}

} /* namespace TransModel */
