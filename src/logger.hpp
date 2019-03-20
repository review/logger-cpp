
#ifndef REVIEW_LOGGER_HPP
#define REVIEW_LOGGER_HPP

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <unordered_map>

namespace review {

struct triplet
{
  double x, y, z;
};

struct rgba
{
  double r, g, b, a;
};

class logger
{

  // private:
public:
  json log_data_;
  std::unordered_map<std::string, std::array<double, 7>> positions_;

  // Utilitiy method called by add_*
  void add_object(const std::string& name,
                  const std::string& type,
                  const triplet& scale,
                  const rgba& color)
  {
    log_data_["objects"].push_back({
      { "name", name },
      { "mesh", type },
      { "scale", { scale.x, scale.y, scale.z } },
      { "material", {{ "color", { color.r, color.g, color.b, color.a }} } }
    });
  }

public:
  logger(double step, const std::string& name = "")
  {
    if (name != "") log_data_["name"] = name;
    log_data_["timeStep"] = step;
  }

  void add_sphere(const std::string& n, double r, const rgba& c = { 1, 1, 1, 1 })
  {
    add_object(n, "sphere", triplet{ r, r, r }, c);
  }

  void add_ellipsoid(const std::string& n, double x, double y, double z, const rgba& c = { 1, 1, 1, 1 })
  {
    add_object(n, "sphere", triplet{ x / 2.0, y / 2.0, z / 2.0 }, c);
  }

  void add_box(const std::string& n, double x, double y, double z, const rgba& c = { 1, 1, 1, 1 })
  {
    add_object(n, "cube", triplet{ x, y, z }, c);
  }

  void add_cylinder(const std::string& n, double r, double height, const rgba& c = { 1, 1, 1, 1 })
  {
    add_object(n, "cylinder", triplet{ r, height, r }, c);
  }

  void new_frame()
  {
    log_data_["frames"].push_back({});
  }

  void add_to_frame(const std::string& name, double x, double y, double z,
                    double qx, double qy, double qz, double qw)
  {
    if (enough_motion(name, x, y, z, qx, qy, qz, qw)) {
      log_data_["frames"].back()[name] = {
        { "t", { x, y, z } },
        { "r", { qx, qy, qz, qw } }
      };
      positions_[name] = std::array<double, 7>{ { x, y, z, qx, qy, qz, qw } };
    }
  }

  void add_frame(const std::string& name, double x, double y, double z,
                 double qx, double qy, double qz, double qw)
  {
    new_frame();
    add_to_frame(name, x, y, z, qx, qy, qz, qw);
  }

  auto to_string(bool pretty = true) const
  {
    return log_data_.dump(pretty ? 4 : -1);
  }

  bool enough_motion(const std::string& name,
                     double x, double y, double z,
                     double qx, double qy, double qz, double qw,
                     double trans_tol = 0.01,
                     double quat_tol = 0.01)
  {
    // return true;
    return positions_.find(name) == positions_.end() ||
           abs(x - positions_[name][0]) > trans_tol ||
           abs(y - positions_[name][1]) > trans_tol ||
           abs(z - positions_[name][2]) > trans_tol ||
           abs(qx - positions_[name][3]) > quat_tol ||
           abs(qy - positions_[name][4]) > quat_tol ||
           abs(qz - positions_[name][5]) > quat_tol ||
           abs(qw - positions_[name][6]) > quat_tol;
  }
};

}

#endif
