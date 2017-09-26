/*
 * Stats.cpp
 *
 *  Created on: Nov 6, 2015
 *      Author: nick
 */

#include "boost/filesystem.hpp"

#include "Stats.h"
#include "file_utils.h"

namespace TransModel {

const std::string ARTEvent::header("\"tick\",\"p_id\",\"event_type\"");

void ARTEvent::writeTo(FileOutput& out) {
	out << tick << "," << p_id << "," << (int)type << "\n";
}

const std::string PREPEvent::header("\"tick\",\"p_id\",\"event_type\"");

void PREPEvent::writeTo(FileOutput& out) {
	out << tick << "," << p_id << "," << type << "\n";
}

const std::string TestingEvent::header("\"tick\",\"p_id\",\"result\"");

void TestingEvent::writeTo(FileOutput& out) {
	out << tick << "," << p_id << "," << result << "\n";
}

const std::string DeathEvent::header("\"tick\",\"p_id\",\"age\",\"art_status\",\"cause\"");
const std::string DeathEvent::AGE("AGE");
const std::string DeathEvent::INFECTION("INFECTION");
const std::string DeathEvent::ASM("ASM");

void DeathEvent::writeTo(FileOutput& out) {
	out << tick << "," << p_id << "," << age << "," << art_status << "," << cause << "\n";
}

const std::string Biomarker::header("\"tick\",\"p_id\",\"viral_load\",\"cd4_count\",\"art_status\"");

void Biomarker::writeTo(FileOutput& out) {
	out << tick << "," << p_id << "," << viral_load << "," << cd4 << "," << on_art << "\n";
}

const std::string InfectionEvent::header(
		"\"tick\",\"infector\",\"p1_age\",\"p1_viral_load\",\"p1_cd4\",\"p1_art_status\",\"p1_on_prep\",\"p1_infectivity\","
				"\"condom_used\",\"infectee\",\"p2_age\",\"p2_viral_load\",\"p2_cd4\",\"p1_on_prep\",\"network_type\"");

void InfectionEvent::writeTo(FileOutput& out) {
	out << tick << "," << p1_id << "," << p1_age << "," << p1_viral_load << "," << p1_cd4 << "," << p1_art << ","
			<< p1_on_prep << "," << p1_infectivity << "," << condom_used << "," << p2_id << "," << p2_age << ","
			<< p2_viral_load << "," << p2_cd4 << "," << p2_on_prep << "," << network_type << "\n";
}

const std::string PartnershipEvent::header("\"tick\",\"edge_id\",\"p1\",\"p2\",\"type\",\"network_type\"");

PartnershipEvent::PartnershipEvent(double tick, unsigned int edge_id, int p1, int p2, PEventType type, int net_type) :
		tick_(tick), edge_id_(edge_id), p1_id(p1), p2_id(p2), type_ { type }, network_type { net_type } {
}

void PartnershipEvent::writeTo(FileOutput& out) {
	out << tick_ << "," << edge_id_ << "," << p1_id << "," << p2_id << "," << static_cast<int>(type_) << "," << network_type << "\n";
}

const std::string Counts::header(
		"\"tick\",\"entries\",\"max_age_exits\",\"infection_deaths\",\"asm_deaths\","
		"\"infected_via_transmission\",\"infected_via_transmission_u26\",\"infected_via_transmission_gte26\","
		"\"infected_externally\",\"infected_at_entry\","
		"\"uninfected\",\"uninfected_u26\",\"uninfected_gte26\","
		"\"steady_edge_count\",\"casual_edge_count\","
		"\"vertex_count\",\"vertex_count_u26\",\"vertex_count_gte26\","
		"\"overlaps\",\"sex_acts\",\"casual_sex_acts\","
		"\"sd_casual_sex_with_condom\",""\"sd_casual_sex_without_condom\","
		"\"sc_casual_sex_with_condom\",""\"sc_casual_sex_without_condom\","
		"\"steady_sex_acts\","
		"\"sd_steady_sex_with_condom\",\"sd_steady_sex_without_condom\","
		"\"sc_steady_sex_with_condom\",\"sc_steady_sex_without_condom\","
		"\"on_art\",\"on_prep\"");

void Counts::writeTo(FileOutput& out) {
	out << tick << "," << entries << "," << age_deaths << "," << infection_deaths << "," << asm_deaths << ","
			<< internal_infected << ","	<< infected_via_transmission_u26 << "," << infected_via_transmission_gte26 << ","
			<< external_infected << "," << infected_at_entry << ","
			<< uninfected << "," << uninfected_u26 << "," << uninfected_gte26 << ","
			<< main_edge_count << "," << casual_edge_count << ","
			<< size << "," << vertex_count_u26 << "," << vertex_count_gte26 << ","
			<< overlaps << "," << sex_acts << "," << casual_sex_acts << ","
			<< sd_casual_sex_with_condom << "," << sd_casual_sex_without_condom << ","
			<< sc_casual_sex_with_condom << "," << sc_casual_sex_without_condom << ","
			<< steady_sex_acts << ","
			<< sd_steady_sex_with_condom << "," << sd_steady_sex_without_condom << ","
			<< sc_steady_sex_with_condom << "," << sc_steady_sex_without_condom << ","
			<< on_art << "," << on_prep << "\n";
}

Counts::Counts() :
		tick { 0 }, main_edge_count { 0 }, casual_edge_count { 0 }, size { 0 }, internal_infected { 0 }, external_infected{0}, infected_at_entry { 0 }, uninfected {
				0 }, entries { 0 }, age_deaths { 0 }, infection_deaths { 0 }, asm_deaths{0}, overlaps { 0 }, sex_acts { 0 },
				casual_sex_acts{0}, steady_sex_acts{0}, sd_casual_sex_with_condom{0}, sd_casual_sex_without_condom{0},
				sd_steady_sex_with_condom{0}, sd_steady_sex_without_condom {0},
				sc_casual_sex_with_condom{0}, sc_casual_sex_without_condom{0},
				sc_steady_sex_with_condom{0}, sc_steady_sex_without_condom {0},
				on_art{0}, on_prep{0}, uninfected_u26{0}, uninfected_gte26{0},
				infected_via_transmission_u26{0}, infected_via_transmission_gte26{0},
				vertex_count_u26{0}, vertex_count_gte26{0}

{
}

void Counts::reset() {
	tick = 0;
	main_edge_count = casual_edge_count = size = internal_infected = external_infected = entries = age_deaths = uninfected = infection_deaths = asm_deaths =
			infected_at_entry = 0, sex_acts = 0, casual_sex_acts = 0, steady_sex_acts = 0;
	sd_casual_sex_with_condom = sd_casual_sex_without_condom = 0;
	sd_steady_sex_with_condom = sd_steady_sex_without_condom = 0;
	sc_casual_sex_with_condom = sc_casual_sex_without_condom = 0;
	sc_steady_sex_with_condom = sc_steady_sex_without_condom = 0;
	on_art = on_prep = 0;
	uninfected_u26 = uninfected_gte26 = infected_via_transmission_u26 = infected_via_transmission_gte26 = 0;
	vertex_count_u26 = vertex_count_gte26 = 0;
	overlaps = 0;
}

void Counts::incrementInfected(PersonPtr& p) {
	++internal_infected;
	// < 26, >= 26
	if (p->age() < 26) ++infected_via_transmission_u26;
	else ++infected_via_transmission_gte26;
}

void Counts::incrementUninfected(PersonPtr& p) {
	++uninfected;
	// < 26, >= 26
	if (p->age() < 26) ++uninfected_u26;
	else ++uninfected_gte26;
}

void Counts::incrementVertexCount(PersonPtr p) {
	++size;
	// < 26, >= 26
	if (p->age() < 26) ++vertex_count_u26;
	else ++vertex_count_gte26;
}

Stats* Stats::instance_ = nullptr;

Stats::Stats(std::shared_ptr<StatsWriter<Counts>> counts, std::shared_ptr<StatsWriter<PartnershipEvent>> pevents,
		std::shared_ptr<StatsWriter<InfectionEvent>> infection_event_writer, std::shared_ptr<StatsWriter<Biomarker>> bio_writer,
		std::shared_ptr<StatsWriter<DeathEvent>> death_event_writer, const std::string& person_data_fname,
		std::shared_ptr<StatsWriter<TestingEvent>> testing_event_writer, std::shared_ptr<StatsWriter<ARTEvent>> art_writer,
		std::shared_ptr<StatsWriter<PREPEvent>> prep_writer) :
		counts_writer { counts }, current_counts { }, pevent_writer { pevents }, ievent_writer { infection_event_writer }, biomarker_writer {
				bio_writer }, death_writer { death_event_writer }, tevent_writer{testing_event_writer}, art_event_writer {art_writer}, prep_event_writer{prep_writer},
				pd_recorder{person_data_fname, 1000}
			 {
}

Stats::~Stats() {
}

void Stats::resetForNextTimeStep() {
	counts_writer->addOutput(current_counts);
	current_counts.reset();
}

void Stats::recordARTEvent(double time, int p_id, bool onART) {
	art_event_writer->addOutput(ARTEvent{time, p_id, onART});
}

void Stats::recordPREPEvent(double time, int p_id, int type) {
	prep_event_writer->addOutput(PREPEvent{time, p_id, type});
}

void Stats::recordPartnershipEvent(double t, unsigned int edge_id, int p1, int p2, PartnershipEvent::PEventType event_type, int net_type) {
	pevent_writer->addOutput(PartnershipEvent { t, edge_id, p1, p2, event_type, net_type });
}

void Stats::recordTestingEvent(double time, int p_id, bool result) {
	tevent_writer->addOutput(TestingEvent{time, p_id, result});
}

void Stats::recordInfectionEvent(double time, const PersonPtr& p) {
	InfectionEvent evt;
	evt.tick = time;
	evt.p1_id = p->id();
	evt.p1_age = p->age();
	evt.p1_art = p->infectionParameters().art_status;
	evt.p1_cd4 = p->infectionParameters().cd4_count;
	evt.p1_infectivity = p->infectivity();
	evt.p1_viral_load = p->infectionParameters().viral_load;
	evt.p1_on_prep = p->isOnPrep();
	evt.p2_id = -1;
	evt.p2_age = 0;
	evt.p2_cd4 = 0;
	evt.p2_viral_load = 0;
	evt.p2_on_prep = false;
	evt.condom_used = false;
	evt.network_type = -1;
	ievent_writer->addOutput(evt);
}

void Stats::recordInfectionEvent(double time, const PersonPtr& p1, const PersonPtr& p2, bool condom, int net_type) {
	InfectionEvent evt;
	evt.tick = time;
	evt.p1_id = p1->id();
	evt.p1_age = p1->age();
	evt.p1_art = p1->infectionParameters().art_status;
	evt.p1_cd4 = p1->infectionParameters().cd4_count;
	evt.p1_infectivity = p1->infectivity();
	evt.p1_viral_load = p1->infectionParameters().viral_load;
	evt.p1_on_prep = p1->isOnPrep();
	evt.p2_id = p2->id();
	evt.p2_age = p2->age();
	evt.p2_cd4 = p2->infectionParameters().cd4_count;
	evt.p2_viral_load = p2->infectionParameters().viral_load;
	evt.p2_on_prep = p2->isOnPrep();
	evt.condom_used = condom;
	evt.network_type = net_type;
	ievent_writer->addOutput(evt);
}

void Stats::recordBiomarker(double time, const PersonPtr& person) {
	Biomarker marker;
	marker.tick = time;
	marker.cd4 = person->infectionParameters().cd4_count;
	marker.on_art = person->infectionParameters().art_status;
	marker.p_id = person->id();
	marker.viral_load = person->infectionParameters().viral_load;
	biomarker_writer->addOutput(marker);
}

void Stats::recordDeathEvent(double time, const PersonPtr& person, const std::string& cause) {
	DeathEvent event;
	event.tick = time;
	event.age = person->age();
	event.p_id = person->id();
	event.art_status = person->infectionParameters().art_status;
	event.cause = cause;
	death_writer->addOutput(event);
}

} /* namespace TransModel */
