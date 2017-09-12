/*
 * PersonCreator.cpp
 *
 *  Created on: Feb 15, 2016
 *      Author: nick
 */

#include "Parameters.h"

#include "PersonCreator.h"

#include "adherence_functions.h"
#include "Diagnoser.h"
#include "Stats.h"

using namespace Rcpp;

namespace TransModel {

PersonCreator::PersonCreator(std::shared_ptr<TransmissionRunner>& trans_runner, double detection_window) :
		id(0), trans_runner_(trans_runner), testing_dist(create_testing_dist()), detection_window_(detection_window) {
}

PersonCreator::~PersonCreator() {
}

int calculate_role(int network_type) {
	double insertive =
			network_type == STEADY_NETWORK_TYPE ?
					Parameters::instance()->getDoubleParameter(PR_INSERTIVE_MAIN) :
					Parameters::instance()->getDoubleParameter(PR_INSERTIVE_CASUAL);
	double receptive = (
			network_type == STEADY_NETWORK_TYPE ?
					Parameters::instance()->getDoubleParameter(PR_RECEPTIVE_MAIN) :
					Parameters::instance()->getDoubleParameter(PR_RECEPTIVE_CASUAL)) + insertive;

	double draw = repast::Random::instance()->nextDouble();
	if (draw <= insertive) {
		return INSERTIVE;
	} else if (draw <= receptive) {
		return RECEPTIVE;
	} else {
		return VERSATILE;
	}
}

PersonPtr PersonCreator::operator()(double tick, float age) {
	int status = (int) repast::Random::instance()->getGenerator(CIRCUM_STATUS_BINOMIAL)->next();
	double test_prob = testing_dist.draw(repast::Random::instance()->nextDouble()).next(Parameters::instance()->getDoubleParameter(SIZE_OF_TIMESTEP));
	Diagnoser diagnoser(tick, detection_window_, test_prob);
	PersonPtr person = std::make_shared<Person>(id++, age, status == 1, calculate_role(STEADY_NETWORK_TYPE),
			calculate_role(CASUAL_NETWORK_TYPE), diagnoser);
	person->testable_ = ((int) repast::Random::instance()->getGenerator(NON_TESTERS_BINOMIAL)->next()) == 0;

	AdherenceData data = initialize_prep_adherence();
	PrepParameters prep(PrepStatus::OFF, 0, 0, data);
	person->prep_ = prep;

	return person;
}

PersonPtr PersonCreator::operator()(Rcpp::List& val, double tick) {
	//std::cout << "------------" << std::endl;
	//Rf_PrintValue(val);
	float age = as<float>(val["age"]);
	bool circum_status = as<bool>(val["circum.status"]);
	int role_main = as<int>(val["role_main"]);
	int role_casual = role_main;
	if (val.containsElementNamed("role_casual")) {
		role_casual = as<int>(val["role_casual"]);
	}

	//float next_test_at = tick + as<double>(val["time.until.next.test"]);
	// float detection_window,  unsigned int test_count, test_prob
	double test_prob = 0;
	if (val.containsElementNamed("testing.probability")) {
		test_prob = as<double>(val["testing.probability"]);
	} else {
		TestingDist dist = testing_dist.draw(repast::Random::instance()->nextDouble());
		test_prob = dist.next(Parameters::instance()->getDoubleParameter(SIZE_OF_TIMESTEP));
	}
	Diagnoser diagnoser(detection_window_, as<unsigned int>(val["number.of.tests"]), test_prob);
	PersonPtr person = std::make_shared<Person>(id++, age, circum_status, role_main, role_casual, diagnoser);
	person->diagnosed_ = as<bool>(val["diagnosed"]);
	person->testable_ = !(as<bool>(val["non.testers"]));
	person->infection_parameters_.cd4_count = as<float>(val["cd4.count.today"]);

	bool infected = as<bool>(val["inf.status"]);
	if (infected) {
		person->infection_parameters_.infection_status = true;
		person->infection_parameters_.time_since_infection = as<float>(val["time.since.infection"]);
		person->infection_parameters_.time_of_infection = as<float>(val["time.of.infection"]);
		person->infection_parameters_.age_at_infection = as<float>(val["age.at.infection"]);
		person->infection_parameters_.dur_inf_by_age = trans_runner_->durInfByAge(
				person->infection_parameters_.age_at_infection);
		person->infection_parameters_.art_status = as<bool>(val["art.status"]);
		if (person->infection_parameters_.art_status) {
			person->infection_parameters_.time_since_art_init = as<float>(val["time.since.art.initiation"]);
			person->infection_parameters_.time_of_art_init = as<float>(val["time.of.art.initiation"]);
			person->infection_parameters_.vl_art_traj_slope = as<float>(val["vl.art.traj.slope"]);
			person->infection_parameters_.cd4_at_art_init = as<float>(val["cd4.at.art.initiation"]);
			person->infection_parameters_.vl_at_art_init = as<float>(val["vl.at.art.initiation"]);
			Stats::instance()->recordARTEvent(tick, person->id(), true);

			if (val.containsElementNamed("adherence.category")) {
				initialize_art_adherence(person, tick, static_cast<AdherenceCategory>(as<int>(val["adherence.category"])));
			} else {
				initialize_art_adherence(person, tick);
			}
		}

		person->infection_parameters_.viral_load = as<float>(val["viral.load.today"]);
	} else {
		//  the prep.status attribute only exists in uninfected persons in the R model
		PrepStatus status = as<bool>(val["prep.status"]) ? PrepStatus::ON : PrepStatus::OFF;
		if (status == PrepStatus::ON) {
			Stats::instance()->recordPREPEvent(tick, person->id(), static_cast<int>(PrepStatus::ON));
		}

		if (val.containsElementNamed("prep.adherence.category")) {
			AdherenceData data = initialize_prep_adherence(static_cast<AdherenceCategory>(as<int>(val["prep.adherence.category"])));
			PrepParameters prep(status, as<double>(val["time.of.prep.initiation"]),
			// add 1 so they spend at least a day on prep and .1 so occurs after main loop
					as<double>(val["time.of.prep.cessation"]) + 1.1, data);
			person->prep_ = prep;
		} else {
			AdherenceData data = initialize_prep_adherence();
			PrepParameters prep(status, as<double>(val["time.of.prep.initiation"]),
			// add 1 so they spend at least a day on prep and .1 so occurs after main loop
					as<double>(val["time.of.prep.cessation"]) + 1.1, data);
			person->prep_ = prep;
		}
	}

	return person;
}

} /* namespace TransModel */
