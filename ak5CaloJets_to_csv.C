#if !defined(__CINT__) && !defined(__MAKECINT__)
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/ChainEvent.h"
#include "DataFormats/Provenance/interface/EventAuxiliary.h"

#include "DataFormats/JetReco/interface/CaloJet.h"
#endif

#include <fstream>
#include <ctime>

using namespace std;

void print_measurement(const time_t started_at, const Long64_t total_events, const Long64_t current_event) {
	time_t current_time	= time(NULL);

	double elapsed		= double(current_time - started_at);
	double per_event	= elapsed / current_event;

	int elapsed_i		= int(elapsed);
	int elapsed_seconds	= elapsed_i % 60;
	int elapsed_minutes	= (elapsed_i / 60) % 60;
	int elapsed_hours	= elapsed_i / 60 / 60;

	int remaining		= int(per_event * (total_events - current_event));
	int remaining_seconds	= remaining % 60;
	int remaining_minutes	= (remaining / 60) % 60;
	int remaining_hours	= remaining / 60 / 60;

	cout	<< current_event << " of " << total_events << " events |   "
		<< "Elapsed: "   << elapsed_hours << "h " << elapsed_minutes << "m " << elapsed_seconds << "sec / "
		<< "Per event: " << per_event << " sec / "
		<< "Remaining: " << remaining_hours << "h " << remaining_minutes << "m " << remaining_seconds << "sec "
		<< endl;

	cout.flush();
}

const vector<string> loadFileIndex(const char *index_filename) {
	ifstream index(index_filename);
	vector<string> result;

	string buffer;
	while (getline(index, buffer)) {
		result.push_back(buffer);
	}

	return result;
}

void downloadCaloJets(const vector<string> filenames, const char *events_csv_filename, const char *jets_csv_filename, const int events_to_process = -1) {
	ofstream events_csv(events_csv_filename);
	ofstream jets_csv(jets_csv_filename);

	events_csv	<< "event_id" << " "
			<< "run_id" << " "
			<< "luminocity_block_id" << " "
			<< "ak5CaloJets_count" << endl;
	
	jets_csv	<< "event_id" << " "
			<< "run_id" << " "
			<< "luminocity_block_id" << " "
			<< "pt" << " "
			<< "eta" << " "
			<< "phi" << " "
			<< "mass" << " "
			<< "vertex_fx" << " "
			<< "vertex_fy" << " "
			<< "vertex_fz" << endl;


	cout << "Init ChainEvents (it may take a long time: several minutes for 30 files as example)..." << endl;
	fwlite::ChainEvent event(filenames);
	fwlite::Handle< vector<reco::CaloJet> > handle;
	int total_events = event.size();
	cout << "Total events: " << total_events << endl;

	int processed_events = 0;
	time_t started_at = time(NULL);

	for (event.toBegin(); !event.atEnd(); ++event) {
		handle.getByLabel(event, "ak5CaloJets");

		if (!handle.isValid()) {
			std::cout << "Invalid handle!" << std::endl;
			return;
		}
		
		edm::EventAuxiliary aux = event.eventAuxiliary();
		events_csv	<< aux.event() << " "
				<< aux.run() << " "
				<< aux.luminosityBlock() << " "
				<< handle->size() << endl;

		vector<reco::CaloJet>::const_iterator jet;
		for (jet = handle->begin(); jet != handle->end(); jet++) {
			jets_csv	<< aux.event() << " "
					<< aux.run() << " "
					<< aux.luminosityBlock() << " "
					<< jet->pt() << " "
					<< jet->eta() << " "
					<< jet->phi() << " "
					<< jet->mass() << " "
					<< jet->vertex().Coordinates().x() << " "
					<< jet->vertex().Coordinates().y() << " "
					<< jet->vertex().Coordinates().z() << endl;
		}

		processed_events++;
		if (processed_events % 1000 == 0) {
			print_measurement(started_at, total_events, processed_events);
		}

		if (events_to_process != -1) {
			if (processed_events >= events_to_process) {
				break;
			}
		}
	}

	print_measurement(started_at, total_events, total_events);

	events_csv.close();
	jets_csv.close();
}
