/*
 *  Copyright 2020 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "pending_task_safety_flag.h"

namespace basic_comm {

// static
std::shared_ptr<PendingTaskSafetyFlag> PendingTaskSafetyFlag::CreateInternal(
    bool alive) {
  // Explicit new, to access private constructor.
  return std::shared_ptr<PendingTaskSafetyFlag>(
      new PendingTaskSafetyFlag(alive));
}

// static
std::shared_ptr<PendingTaskSafetyFlag> PendingTaskSafetyFlag::Create() {
  return CreateInternal(true);
}

std::shared_ptr<PendingTaskSafetyFlag>
PendingTaskSafetyFlag::CreateDetached() {
  std::shared_ptr<PendingTaskSafetyFlag> safety_flag = CreateInternal(true);
  return safety_flag;
}

std::shared_ptr<PendingTaskSafetyFlag>
PendingTaskSafetyFlag::CreateDetachedInactive() {
  std::shared_ptr<PendingTaskSafetyFlag> safety_flag = CreateInternal(false);
  return safety_flag;
}

void PendingTaskSafetyFlag::SetNotAlive() {
  alive_ = false;
}

void PendingTaskSafetyFlag::SetAlive() {
  alive_ = true;
}

bool PendingTaskSafetyFlag::alive() const {
  return alive_;
}

}  // namespace webrtc
