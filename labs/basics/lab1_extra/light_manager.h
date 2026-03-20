#ifndef LAB_LIGHT_MANAGER_H_
#define LAB_LIGHT_MANAGER_H_

#include <QMap>
#include <QString>
#include <vector>
#include "light_state.h"

class LightManager {
 public:
  LightManager() { InitializeDefaultRooms(); }

  void CreateRoom(const QString& room_name, const LightState& initial_state) {
    rooms_[room_name] = initial_state;
  }

  bool HasRoom(const QString& room_name) const { return rooms_.contains(room_name); }

  LightState GetRoomState(const QString& room_name) const {
    return rooms_.value(room_name, LightState());
  }

  void SetRoomState(const QString& room_name, const LightState& state) {
    if (rooms_.contains(room_name)) {
      rooms_[room_name] = state;
    }
  }

  QStringList GetAllRoomNames() const { return rooms_.keys(); }

  int GetRoomCount() const { return rooms_.size(); }

  std::vector<int> GetNonMasteredRoomIndices() const {
    // For potential future use in random room selection
    std::vector<int> indices;
    for (int i = 0; i < rooms_.size(); ++i) {
      indices.push_back(i);
    }
    return indices;
  }

 private:
  void InitializeDefaultRooms() {
    rooms_["Living Room"] = LightState(100, 5000, true);
    rooms_["Bedroom"] = LightState(50, 3000, true);
    rooms_["Kitchen"] = LightState(100, 6500, true);
    rooms_["Bathroom"] = LightState(80, 5500, true);
    rooms_["Office"] = LightState(90, 6000, true);
  }

  QMap<QString, LightState> rooms_;
};

#endif  // LAB_LIGHT_MANAGER_H_
