#include "TelemetryReplay.h"
#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace boost::filesystem;

/**
 * Constructor for the TelemetryReplay class
 * @param path location of the file/directory to read.
 */
TelemetryReplay::TelemetryReplay(std::string &path) :
        path_{path}, readings_{} {}

void TelemetryReplay::startup() {
    if (exists(path_)) {
        if (is_regular_file((path_))) {
            parseFile(path_);
        } else if (is_directory((path_))) {
            vector<path> directoryEntries;
            copy(directory_iterator(path_), directory_iterator(), std::back_inserter(directoryEntries));
            sort(directoryEntries.begin(), directoryEntries.end());

            for (path &f: directoryEntries) {
                if (is_regular_file(f)) {
                    parseFile(f);
                }
            }
        } else {
            throw (std::runtime_error(path_.string() + " exists, but is not a regular file or directory\n"));
        }
    } else {
        throw (std::runtime_error(path_.string() + " does not exist\n"));
    }

    lastReadingIter_ = readings_.begin();
}

vector<RocketEvent> TelemetryReplay::pollEvents() {
    return vector<RocketEvent>();
}

vector<TelemetryReading> TelemetryReplay::pollData() {
    vector<TelemetryReading> vec;

    for (int i = 0; i < 14; ++i) {
        vec.push_back(*lastReadingIter_++);
    }

    return vec;
}

void TelemetryReplay::parseFile(boost::filesystem::path p) {
    boost::filesystem::ifstream ifs{p, ios::in};

    string reading;
    while (getline(ifs, reading)) {
        std::vector<std::string> values;
        boost::split(values, reading, boost::is_any_of("\t "),
                     boost::algorithm::token_compress_mode_type::token_compress_on);

        if (values.size() != 15) {
            cout << "Invalid reading" << endl;
        }
        try {

            auto it = values.begin();
            uint32_t seqNumber = std::stoul(*it++);
            uint32_t timestamp = std::stoul(*it++);
            double altitude = std::stod(*it++);
            double air_speed = std::stod(*it++);
            double pressure = std::stod(*it++);
            double temperature = std::stod(*it++);
            double ax = std::stod(*it++);
            double ay = std::stod(*it++);
            double az = std::stod(*it++);
            double gx = std::stod(*it++);
            double gy = std::stod(*it++);
            double gz = std::stod(*it++);
            double mx = std::stod(*it++);
            double my = std::stod(*it++);
            double mz = std::stod(*it++);

            TelemetryReading r{
                    timestamp, altitude, XYZReading{ax, ay, az}, XYZReading{mx, my, mz}, XYZReading{gx, gy, gz},
                    pressure, temperature, air_speed, seqNumber};
            readings_.push_back(r);

        } catch (std::invalid_argument &e) {

        }
    }
}

TimedData TelemetryReplay::parseLine(string &reading) {

}


