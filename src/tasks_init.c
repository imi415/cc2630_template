#include "FreeRTOS.h"
#include "task.h"

#include "user_tasks.h"

void u_CreateUserTasks(void) {
    xTaskCreate(vUserTaskHello, "TaskHello", 128, NULL, 15, &xUserTaskHelloHandle);
}