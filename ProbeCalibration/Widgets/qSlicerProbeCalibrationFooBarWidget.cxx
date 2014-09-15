/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// FooBar Widgets includes
#include "qSlicerProbeCalibrationFooBarWidget.h"
#include "ui_qSlicerProbeCalibrationFooBarWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ProbeCalibration

class qSlicerProbeCalibrationFooBarWidgetPrivate
: public Ui_qSlicerProbeCalibrationFooBarWidget
{

    Q_DECLARE_PUBLIC(qSlicerProbeCalibrationFooBarWidget);
        
protected:
    qSlicerProbeCalibrationFooBarWidget * const q_ptr;

public:
    qSlicerProbeCalibrationFooBarWidgetPrivate(
            qSlicerProbeCalibrationFooBarWidget& object);
    virtual void setupUi(qSlicerProbeCalibrationFooBarWidget*);
};

// --------------------------------------------------------------------------

qSlicerProbeCalibrationFooBarWidgetPrivate
::qSlicerProbeCalibrationFooBarWidgetPrivate(
        qSlicerProbeCalibrationFooBarWidget& object)
: q_ptr(&object)
{
}

// --------------------------------------------------------------------------

void qSlicerProbeCalibrationFooBarWidgetPrivate
::setupUi(qSlicerProbeCalibrationFooBarWidget* widget)
{
    this->Ui_qSlicerProbeCalibrationFooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerProbeCalibrationFooBarWidget methods

//-----------------------------------------------------------------------------

qSlicerProbeCalibrationFooBarWidget
::qSlicerProbeCalibrationFooBarWidget(QWidget* parentWidget)
: Superclass(parentWidget)
, d_ptr(new qSlicerProbeCalibrationFooBarWidgetPrivate(*this))
{
    Q_D(qSlicerProbeCalibrationFooBarWidget);
    d->setupUi(this);
}

//-----------------------------------------------------------------------------

qSlicerProbeCalibrationFooBarWidget
::~qSlicerProbeCalibrationFooBarWidget()
{
}
