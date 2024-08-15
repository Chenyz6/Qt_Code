#include "secondpagecontroller.h"

QSecondPageController *QSecondPageController::m_instance = new QSecondPageController();

QSecondPageController *QSecondPageController::GetInstance()
{
    return m_instance;
}

QSecondPageController::QSecondPageController()
{

}
