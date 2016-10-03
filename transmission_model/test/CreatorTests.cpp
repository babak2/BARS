/*
 * CD4ViralTests.cpp
 *
 *  Created on: Dec 2, 2015
 *      Author: nick
 */

#include "gtest/gtest.h"

#include "PersonCreator.h"
#include "RInstance.h"
#include "TransmissionRunner.h"
#include "StatsBuilder.h"

using namespace TransModel;
using namespace Rcpp;

TEST(CreatorTests, TestInfectedPersonCreationNoART) {
	std::string cmd = "load(file=\"../test_data/initialized-model.RData\")";
	RInstance::rptr->parseEvalQ(cmd);
	List rnet = as<List>((*RInstance::rptr)["n0"]);
	List val = as<List>(rnet["val"]);
	List p_list = as<List>(val[101]);
	// make sure I have the right one
	ASSERT_NEAR(61.87456, as<double>(p_list["age"]), 0.00001);

	std::vector<float> dur_inf { 10, 20, 30, 40 };
	std::shared_ptr<TransmissionRunner> runner = std::make_shared<TransmissionRunner>(1, 1, 1, dur_inf);
	PersonCreator creator(runner, 0.5, 1);
	PersonPtr person = creator(p_list, 1);
	ASSERT_FALSE(person->isCircumcised());
	ASSERT_TRUE(person->isInfected());
	ASSERT_EQ(0, person->role());
	ASSERT_NEAR(61.87456f, person->age(), 0.00001);
	ASSERT_FALSE(person->isDiagnosed());
	// created at tick 1 so 43 from tick 1
	ASSERT_EQ(43, person->timeUntilNextTest(1));
	ASSERT_TRUE(person->isTestable());
	ASSERT_FALSE(person->isOnPrep());

	InfectionParameters params = person->infectionParameters();
	ASSERT_FALSE(params.art_status);
	ASSERT_FLOAT_EQ(294, params.time_since_infection);
	ASSERT_FLOAT_EQ(-294, params.time_of_infection);
	ASSERT_FLOAT_EQ(61.87456, params.age_at_infection);
	ASSERT_FLOAT_EQ(455.2354, params.cd4_count);
	ASSERT_TRUE(isnan(params.time_since_art_init));
	ASSERT_TRUE(isnan(params.time_of_art_init));
	ASSERT_FLOAT_EQ(4.2f, params.viral_load);
	ASSERT_TRUE(isnan(params.vl_art_traj_slope));
	ASSERT_TRUE(isnan(params.vl_at_art_init));
	ASSERT_TRUE(isnan(params.cd4_at_art_init));
}

TEST(CreatorTests, TestUninfectedPersonCreation) {
	std::string cmd = "load(file=\"../test_data/initialized-model.RData\")";
	RInstance::rptr->parseEvalQ(cmd);
	List rnet = as<List>((*RInstance::rptr)["n0"]);
	List val = as<List>(rnet["val"]);
	List p_list = as<List>(val[0]);
	// make sure I have the right one
	ASSERT_NEAR(43.92679, as<double>(p_list["age"]), 0.00001);

	std::vector<float> dur_inf { 10, 20, 30, 40 };
	std::shared_ptr<TransmissionRunner> runner = std::make_shared<TransmissionRunner>(1, 1, 1, dur_inf);
	PersonCreator creator(runner, 0.5, 1);
	PersonPtr person = creator(p_list, 1);
	ASSERT_FALSE(person->isCircumcised());
	ASSERT_FALSE(person->isInfected());
	ASSERT_EQ(1, person->role());
	ASSERT_NEAR(43.92679f, person->age(), 0.00001);
	ASSERT_TRUE(person->isTestable());
	ASSERT_TRUE(person->isOnPrep());

	InfectionParameters params = person->infectionParameters();
	ASSERT_FALSE(params.art_status);
	ASSERT_TRUE(isnan(params.time_since_infection));
	ASSERT_TRUE(isnan(params.time_of_infection));
	ASSERT_TRUE(isnan(params.age_at_infection));
	ASSERT_FLOAT_EQ(518.0f, params.cd4_count);
	ASSERT_TRUE(isnan(params.time_since_infection));
	ASSERT_TRUE(isnan(params.time_since_art_init));
	ASSERT_TRUE(isnan(params.time_of_art_init));
	ASSERT_FLOAT_EQ(0, params.viral_load);
	ASSERT_TRUE(isnan(params.vl_art_traj_slope));
	ASSERT_TRUE(isnan(params.vl_at_art_init));
	ASSERT_TRUE(isnan(params.cd4_at_art_init));
}

TEST(CreatorTests, TestInfectedPersonCreationART) {
	std::string cmd = "load(file=\"../test_data/initialized-model.RData\")";
	RInstance::rptr->parseEvalQ(cmd);
	List rnet = as<List>((*RInstance::rptr)["n0"]);
	List val = as<List>(rnet["val"]);
	List p_list = as<List>(val[16]);
	// make sure I have the right one
	ASSERT_NEAR(17.89907, as<double>(p_list["age"]), 0.00001);

	std::vector<float> dur_inf { 10, 20, 30, 40 };
	std::shared_ptr<TransmissionRunner> runner = std::make_shared<TransmissionRunner>(1, 1, 1, dur_inf);
	PersonCreator creator(runner, 0.5, 1);
	PersonPtr person = creator(p_list, 1);
	ASSERT_FALSE(person->isCircumcised());
	ASSERT_TRUE(person->isInfected());
	ASSERT_EQ(1, person->role());
	ASSERT_NEAR(17.89907f, person->age(), 0.00001);

	ASSERT_TRUE(person->isDiagnosed());
	// should always be 0 for diagnosed person
	ASSERT_EQ(0, person->timeUntilNextTest(1));
	ASSERT_TRUE(person->isTestable());
	ASSERT_FALSE(person->isOnPrep());

	InfectionParameters params = person->infectionParameters();
	ASSERT_TRUE(params.art_status);
	ASSERT_FLOAT_EQ(2134, params.time_since_infection);
	ASSERT_FLOAT_EQ(-2134, params.time_of_infection);
	ASSERT_FLOAT_EQ(17.89907f, params.age_at_infection);
	ASSERT_FLOAT_EQ(257.4882f, params.cd4_count);
	ASSERT_EQ(0, params.time_since_art_init);
	ASSERT_EQ(0, params.time_of_art_init);
	ASSERT_FLOAT_EQ(4.276811f, params.viral_load);
	ASSERT_NEAR(0.02148201f, params.vl_art_traj_slope, 0.000001);
	ASSERT_FLOAT_EQ(4.276811f, params.vl_at_art_init);
	ASSERT_FLOAT_EQ(0, params.cd4_at_art_init);
}

TEST(CreatorTests, TestCreatorFromSavedNet) {
	std::string cmd = "nw.test <- readRDS(file=\"../test_data/network.RDS\")";
	RInstance::rptr->parseEvalQ(cmd);
	List rnet = as<List>((*RInstance::rptr)["nw.test"]);
	std::vector<float> dur_inf { 10, 20, 30, 40 };
	std::shared_ptr<TransmissionRunner> runner = std::make_shared<TransmissionRunner>(1, 1, 1, dur_inf);
	PersonCreator creator(runner, 0.5, 1);
	List val = as<List>(rnet["val"]);

	// will throw exception if there's an issue
	for (auto& sexp : val) {
		List v = as<List>(sexp);
		creator(v, 1);
	}
}

void create_stats() {

	StatsBuilder builder("/dev");
	builder.countsWriter("null");
	builder.partnershipEventWriter("null");
	builder.infectionEventWriter("null");
	builder.biomarkerWriter("null");
	builder.deathEventWriter("null");
	builder.personDataRecorder("null");
	builder.testingEventWriter("null");
	builder.createStatsSingleton();
}

TEST(CreatorTests, TestDiagnosis) {
	create_stats();
	std::string cmd = "load(file=\"../test_data/initialized-model.RData\")";
	RInstance::rptr->parseEvalQ(cmd);
	List rnet = as<List>((*RInstance::rptr)["n0"]);
	List val = as<List>(rnet["val"]);
	List p_list = as<List>(val[0]);
	p_list["time.until.next.test"] = 528;
	p_list["lag.bet.diagnosis.and.art.init"] = 0;

	std::vector<float> dur_inf { 10, 20, 30, 40 };
	std::shared_ptr<TransmissionRunner> runner = std::make_shared<TransmissionRunner>(1, 1, 1, dur_inf);

	// detection window = 10
	PersonCreator creator(runner, 0.5, 10);
	double tick = 2;
	PersonPtr person = creator(p_list, tick);

	float next_test_at = tick + as<double>(p_list["time.until.next.test"]);
	ASSERT_EQ(528 + tick, next_test_at);
	person->infect(50, tick);
	// person should diagnose true at next_text_at, detection window is 10 which is long
	// before next_test_at
	ASSERT_FALSE(person->diagnose(100));
	ASSERT_FALSE(person->diagnose(next_test_at - 1));
	ASSERT_TRUE(person->diagnose(next_test_at));

	p_list["time.until.next.test"] = 5;
	PersonPtr p2 = creator(p_list, tick);
	p2->infect(50, tick);
	ASSERT_EQ(4, p2->timeUntilNextTest(3));
	// test should be false at next_test_at (7)
	// because detection window not yet reached
	ASSERT_FALSE(p2->diagnose(7));
	next_test_at = p2->timeUntilNextTest(7);
	// do the next test at the max of the next testing data
	// or the detection window to insure a positive test
	double test_at = std::max(7 + next_test_at, 12.0f);
	ASSERT_TRUE(p2->diagnose(test_at));
	// should always be 0 if diagnosed
	ASSERT_EQ(0, p2->timeUntilNextTest(50));
	ASSERT_TRUE(p2->isDiagnosed());
}