// Part A: This is an extension task that requires you to decode sensor data from CAN log files.
// CAN (Controller Area Network) is a communication standard used in automotive applications (including Redback cars)
// to allow communication between sensors and controllers.
//
// Your Task: Using the signal definitions in SteeringBench.dbc, read each CAN capture in data/
// and turn it into a CSV with one row per decoded frame:
// t,u_commanded,y_measured
// eg:
// 0,15.0,0.0
// 0.005,15.0,0.0
// ...
// where t is the frame timestamp minus the first kept frame's timestamp (s), u_commanded is
// the decoded CmdAngularRate (deg/s), and y_measured is the decoded MeasuredAngle (deg).
// The above values are not real numbers; they are only there to show the expected data output format.
// Do this for all three captures:
// data/step_test.log       ->  data/step_test.csv
// data/reversal_test.log   ->  data/reversal_test.csv
// data/deadband_test.log   ->  data/deadband_test.csv
//
// The Row type, writeCsv(), and main() below are provided -- they loop the three logs, call your
// decodeLog(), and write the CSV in exactly the format above. You just need to implement decodeLog().
//
// You do not need to use any external libraries. Use the resources below to understand how to
// extract sensor data.
// Hint: Think about manual bit masking and shifting, data types required,
// what formats are used to represent values, etc.
// Resources:
// https://www.csselectronics.com/pages/can-bus-simple-intro-tutorial
// https://www.csselectronics.com/pages/can-dbc-file-database-intro
//
// Sanity check: plot your CSVs (python3 plot_data.py) and compare against the pre-plotted
// data/*.png files -- they should match.
//
// Build & run (from the TA/ folder):
//     c++ -std=c++17 Question-A.cc -o decode
//     ./decode

#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

// One output row.
struct Row {
    double t;            // seconds since the first kept frame
    double u_commanded;  // deg/s
    double y_measured;   // deg
};

// Read the candump log at `path` and return one Row per STEER_ActuatorLog frame, in order.
// Push one Row{t, u_commanded, y_measured} per kept frame.
std::vector<Row> decodeLog(const std::string& path) {
    std::vector<Row> rows;

    std::ifstream canLog(path);
    std::string line;
    double firstTimestamp;

    while (std::getline(canLog, line)) {
        double timestamp = std::stod(line.substr(1));
        if (rows.empty()) firstTimestamp = timestamp;

        size_t hashPos = line.find('#');
        std::string hexPayload = line.substr(hashPos + 1);
        uint64_t canData = std::stoull(hexPayload, nullptr, 16);

        // read little endian
        uint16_t rawData[4];
        for (int i = 0; i < 4; i++) {
            uint16_t lowByte = (canData >> (8 * (7 - 2 * i))) & 0xFF;  
            uint16_t highByte = (canData >> (8 * (6 - 2 * i))) & 0xFF;

            rawData[i] = lowByte | (highByte << 8);
        }

        // SG_ MeasuredAngle : 0|16@1- (0.1,0) [0|0] "deg" Vector__XXX
        double MeasuredAngle = (int16_t)rawData[0] * 0.1;

        // SG_ CmdAngularRate : 16|16@1- (0.1,0) [0|0] "deg/s" Vector__XXX
        double CmdAngularRate = (int16_t)rawData[1] * 0.1;

        // SG_ SupplyMilliVolts : 32|16@1+ (1,0) [0|0] "mV" Vector__XXX
        double SupplyMilliVolts = rawData[2];

        // SG_ ActuatorTemp : 48|16@1- (0.01,-40) [0|0] "degC" Vector__XXX
        double ActuatorTemp = (int16_t)rawData[3] * 0.1 - 40;

        rows.push_back(Row{
            timestamp - firstTimestamp,
            CmdAngularRate,
            MeasuredAngle
        });
    }

    return rows;
}

// Provided -- writes the rows to a CSV in the required format. Do not change.
void writeCsv(const std::string& path, const std::vector<Row>& rows) {
    std::ofstream f(path);
    f << "t,u_commanded,y_measured\n";
    for (const Row& r : rows)
        f << r.t << "," << r.u_commanded << "," << r.y_measured << "\n";
}

// Provided -- runs decodeLog() + writeCsv() for each of the three captures.
int main() {
    const char* names[] = {"step_test", "reversal_test", "deadband_test"};
    for (const char* n : names) {
        const std::string in  = std::string("data/") + n + ".log";
        const std::string out = std::string("data/") + n + ".csv";
        const std::vector<Row> rows = decodeLog(in);
        writeCsv(out, rows);
        std::printf("%-14s %6zu frames -> %s\n", n, rows.size(), out.c_str());
    }
    return 0;
}


std::vector<Row> decodeLog(const std::string& path) {
    std::vector<Row> rows;

    // 1. Open file at path

    // 2. Read one text line at a time
    while (...) {

        // 3. Parse the line:
        //    timestamp
        //    CAN ID
        //    8-byte CAN payload

        // 4. Ignore frames that aren't CAN ID 512 / 0x200

        // 5. Decode payload using DBC:
        //    bytes 0-1 -> MeasuredAngle
        //    bytes 2-3 -> CmdAngularRate

        // 6. Apply scale of 0.1

        // 7. Calculate relative time:
        //    current timestamp - first kept timestamp

        // 8. Add:
        rows.push_back(Row{t, u_commanded, y_measured});
    }

    return rows;
}