
#ifndef REVIEW_LOGGER_HPP
#define REVIEW_LOGGER_HPP


#include "json.hpp"
using json = nlohmann::json;

#include <unordered_map>
#include <iostream>


namespace review
{

struct triplet {
    double x, y, z;
};

class logger
{

// private:
public:
    json log_data_;
    std::unordered_map<std::string, std::array<double, 7>> positions_;

    void add_object(const std::string & name, const std::string & type, const triplet & scale) {
        log_data_["objects"].push_back({
            { "name", name },
            { "mesh", type },
            { "scale", { scale.x, scale.y, scale.z } }
        });
    }

public:
    logger(double step, double stop, const std::string & name = "") {
        if (name != "") {
            log_data_["name"] = name;
        }

        log_data_["timeStep"] = step;
        log_data_["duration"] = stop;
    }

    void add_sphere(const std::string & name, double radius) {
        add_object(name, "sphere", triplet{radius, radius, radius});
    }

    void add_ellipsoid(const std::string & name, double x_size, double y_size, double z_size) {
        add_object(name, "sphere", triplet{x_size/2.0, y_size/2.0, z_size/2.0});
    }

    void add_box(const std::string & name, double x_size, double y_size, double z_size) {
        add_object(name, "cube", triplet{x_size, y_size, z_size});
    }

    void add_cylinder(const std::string & name, double radius, double height) {
        add_object(name, "cylinder", triplet{radius, height, radius});
    }


    void new_frame() {
        log_data_["frames"].push_back({});
    }

    void add_to_frame(const std::string & name,
        double x, double y, double z,
        double qx, double qy, double qz, double qw) {

        if (enough_motion(name, x, y, z, qx, qy, qz, qw)) {
            log_data_["frames"].back()[name] = {
                { "t", {x, y, z}},
                { "r", {qx, qy, qz, qw}}
            };
            positions_[name] = std::array<double, 7>{{x, y, z, qx, qy, qz, qw}};
        }
    }

    void add_frame(const std::string & name,
        double x, double y, double z,
        double qx, double qy, double qz, double qw) {

        new_frame();
        add_to_frame(name, x, y, z, qx, qy, qz, qw);
    }

    auto to_string(bool pretty=true) const {
        return log_data_.dump(pretty ? 4 : -1);
    }

    bool enough_motion(const std::string & name,
        double x, double y, double z,
        double qx, double qy, double qz, double qw,
        double trans_tol = 0.01, double quat_tol = 0.01) {

        // return true;

        return positions_.find(name) == positions_.end()
            || abs(x - positions_[name][0]) > trans_tol
            || abs(y - positions_[name][1]) > trans_tol
            || abs(z - positions_[name][2]) > trans_tol
            || abs(qx - positions_[name][3]) > quat_tol
            || abs(qy - positions_[name][4]) > quat_tol
            || abs(qz - positions_[name][5]) > quat_tol
            || abs(qw - positions_[name][6]) > quat_tol;
    }
};

}

#endif
