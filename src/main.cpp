/*                                                                         80->|
 * main.cpp
 *
 * Modifications: James William Dunn
 *          Date: May 29, 2017
 */

#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "PID.h"
#include <math.h>
#include <chrono>

// for convenience
using json = nlohmann::json;

class Timer1 {
 public:
  using high_resolution_clock = std::chrono::high_resolution_clock;
  using ms = std::chrono::milliseconds;

  Timer1(bool go = true) {
    if (go)	start();
  }

  void start() {
    start_ = high_resolution_clock::now();
  }

  ms duration() {
    high_resolution_clock::time_point now = high_resolution_clock::now();
    ms durat = std::chrono::duration_cast<ms>(now - start_);
    start_ = now;
    return durat;
  }

 private:
  high_resolution_clock::time_point start_;
};




// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != std::string::npos) {
    return "";
  }
  else if (b1 != std::string::npos && b2 != std::string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

float maxSpeed = 0;
long long frameCount = 0;
int faultCount = 0;

/* Tested averaging queues like those used in my behavClone project
float q1[3], q2[9], q3[18];
float tracks[4];
int qi1 = 0, qi2 = 0, qi3 = 0;
float getAvg(float a[], int len) {
  float tot = 0;
  for (int i = 0; i < len; i++)
    tot += a[i];
  return tot / (float)len;
}

int argmin(float anArray[], float key) {
  float value = abs(key - anArray[0]);
  int num = 0;

  for (int x = 0; x < 4; x++) {
    if (abs(key - anArray[x]) < value) {
      value = abs(key - anArray[x]);
      num = x;
    }
  }
  return num;
}*/

int main() {
  uWS::Hub h;

  PID pid;
  Timer1 timer;
  // TODO: Initialize the pid variable.
  //pid.Init(1.09, 0.0, 27.0); initial manual attempts
  //pid.Init(0.15, 0.0, 3.333);
  //pid.Init(0.1, 0.0, 6.0);   // 0.0005 (slightly to the left)     -0.0005 (slightly to the right)
  //pid.Init(0.1, 0.0003, 27.0);
  //pid.Init(0.1, 0.0, 3.0);   // 0.2, 0.001, 15
  //pid.Init(1.0, 0.0, 30.0);
  //pid.Init(0.5, 0.0, 15.0); // q:18, produces oscillation
  //pid.Init(0.1, 0.0, 1.0); // q:18, smooth start fby crash on curve
  //pid.Init(0.1, 0.0, 4.0); // q:18, smoother, no crash, slight oscillations, uncomfortable
  //pid.Init(0.1, 0.0, 7.0); // q:18, back to oscillation
  //pid.Init(0.1, 0.0, 2.5); // q:18, less oscillation, but close to edges
  //pid.Init(0.1, 0.0, 3.25);  // q:3,9,18, smoother, but close to edges    Good
  //pid.Init(0.2, 0.0, 6.25);
  //pid.Init(0.1, 0.0, 3.25);  // attempt w/o q: right turn over line
  //pid.Init(0.1, 0.0, 4.25);  // right turn a tad over line
  //pid.Init(0.1, 0.0, 5.25); // still over right turn, even at 22mph
  //pid.Init(0.2, 0.0, 6.25); // try at 20mph (thr=.17)                     Good
  //pid.Init(0.3, 0.0, 6.25);
  //pid.Init(0.2, 0.001, 16.0); // really good at slow speeds
  //pid.Init(1.0, 0.0001, 32.0); // RC
  //pid.Init(0.088, 0.0, 2.0); going over lines too much
  //pid.Init(1.0, 0.00005, 32.0);
  //pid.Init(2.0, 0.00005, 64.0); RC2
  //pid.Init(2.0, 0.00001, 64.0);
  //pid.Init(1.0, 0.0, 20.0);
  //pid.Init(1.0, 0.00001, 32.0);
  //pid.Init(1.0, 0.00001, 4.0); // try slow

  h.onMessage([&pid, &timer](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2')
    {
      auto s = hasData(std::string(data).substr(0, length));
      if (s != "") {
        auto j = json::parse(s);
        std::string event = j[0].get<std::string>();
        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<std::string>());
          double speed = std::stod(j[1]["speed"].get<std::string>());
          double angle = std::stod(j[1]["steering_angle"].get<std::string>());
      double throttle = 0.05;
      double steer_value = 0.0;
      Timer1::ms duration = timer.duration();
      long long outd = duration.count();
      if (speed > maxSpeed) maxSpeed = speed;
      frameCount++;
      std::cout << "Frm: " << frameCount << " FPS: " << 1000 / outd << " Max: " << maxSpeed;
      
      //std::cout << "cte:" << cte << std::endl;
      //q2[qi2++] = cte; // attempt to smooth out the cte values due to GT anomalies
      //if (qi2 > 3) qi2 = 0;
      //cte = getAvg(q2, 4);
      //std::cout << "cte avg:" << cte << " speed:" << speed << " angle:" << angle << std::endl;
      
      if (cte > -3.8 && cte < 3.8) {  // abs() and fabs() not working
        /*
        * TODO: Calcuate steering value here, remember the steering value is
        * [-1, 1].
        * NOTE: Feel free to play around with the throttle and speed. Maybe use
        * another PID controller to control the speed!
        */
        if (speed > 40.0) { // Fifth gear
          std::cout << " Gear: 5";
          pid.Init(1.0, 0.0, 64.0);
          pid.UpdateError(cte,outd);
          steer_value = pid.TotalError();
          if (cte < -0.6 || cte > 0.6) {
            throttle = -1.0; // BRAKE!!!
            steer_value = std::min(0.2, steer_value);   //  .1 = 2.5 degrees      .15 = 3.75 degrees
            steer_value = std::max(-0.2, steer_value);  //
          }
          else {
            throttle = 1.0; // fly   0.71   0.65
            steer_value = std::min(0.1, steer_value);   //  .1 = 2.5 degrees      .15 = 3.75 degrees
            steer_value = std::max(-0.1, steer_value);  //
          }
        }
        else if (speed > 30.0) {  // Fourth gear
          std::cout << " Gear: 4";
          pid.Init(1.0, 0.0, 64.0);
          pid.UpdateError(cte,outd);
          steer_value = pid.TotalError();
          if (cte < -0.5 || cte > 0.5) {
            throttle = -0.5; // Slow it down and allow for wider steering angle
            steer_value = std::min(0.3, steer_value);   // 0.2 = 7.5 degrees
            steer_value = std::max(-0.3, steer_value);  //
          }
          else {
            throttle = 0.6; // run
            steer_value = std::min(0.1, steer_value);   // 0.1 = 2.5 degrees
            steer_value = std::max(-0.1, steer_value);  //
          }
        }
        else if (speed > 25.0) {  // Third gear
          std::cout << " Gear: 3";
          pid.Init(1.0, 0.0, 32.0);
          pid.UpdateError(cte,outd);
          steer_value = pid.TotalError();
          steer_value = std::min(0.30/2, steer_value);   // 0.3 = 7.5 degrees
          steer_value = std::max(-0.30/2, steer_value);  //
          if (cte < -0.4 || cte > 0.4) throttle = -0.2; // Slow it down
          else throttle = 0.35; // walk
        }
        else if (speed > 18.0) { // Second gear... under .25 throttle 
          std::cout << " Gear: 2";  // is allowed larger turning radius
          pid.Init(1.0, 0.0, 32.0);
          pid.UpdateError(cte,outd);
          steer_value = pid.TotalError();
          steer_value = std::min(0.4, steer_value);   //  0.4 = 10 degrees   
          steer_value = std::max(-0.4, steer_value);  //
          if (cte < -0.3 || cte > 0.3) throttle = 0.0;
          else 
            if (frameCount>400) throttle = 0.25; // crawl
            else throttle = 0.5;
        }
        else { // First gear
          std::cout << " Gear: 1";
          throttle = 1.0; // Start yer engines, give it a boost
        }
        steer_value -= 0.017452; // subtract measured bias for balanced steering
        //steer_value = 0.5 * angle/25.0 + 0.5* steer_value; // use a percentage of steering value

        // DEBUG
        int steerInt = steer_value * 1000;
        steer_value = steerInt / 1000.0;
        std::cout << " Thr: " << throttle << " Str: " << steer_value << " Flt: " << faultCount << std::endl;

        //std::cout << cte << "\t" << steer_value << "\t" << throttle  << std::endl;

        //throttle = 0.1;
        json msgJson;
        msgJson["steering_angle"] = steer_value;
        msgJson["throttle"] = throttle;
        auto msg = "42[\"steer\"," + msgJson.dump() + "]";
        //std::cout << msg << std::endl;
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
      else {
        // reset
        faultCount++;
        std::cout << "resetting" << std::endl;
        std::string msg("42[\"reset\", {}]");
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
        }
      } else {
        // Manual driving
        std::string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }
  });

  // We don't need this since we're not using HTTP but if it's removed the program
  // doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1)
    {
      res->end(s.data(), s.length());
    }
    else
    {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port))  // if IPv4, use: if (h.listen("0.0.0.0",port))
  {
    std::cout << "Listening to port " << port << std::endl;
  }
  else
  {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  h.run();
}
