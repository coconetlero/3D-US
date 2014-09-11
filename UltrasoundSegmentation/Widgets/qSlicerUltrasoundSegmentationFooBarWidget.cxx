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
#include "qSlicerUltrasoundSegmentationFooBarWidget.h"
#include "ui_qSlicerUltrasoundSegmentationFooBarWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_UltrasoundSegmentation
class qSlicerUltrasoundSegmentationFooBarWidgetPrivate
  : public Ui_qSlicerUltrasoundSegmentationFooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerUltrasoundSegmentationFooBarWidget);
protected:
  qSlicerUltrasoundSegmentationFooBarWidget* const q_ptr;

public:
  qSlicerUltrasoundSegmentationFooBarWidgetPrivate(
    qSlicerUltrasoundSegmentationFooBarWidget& object);
  virtual void setupUi(qSlicerUltrasoundSegmentationFooBarWidget*);
};

// --------------------------------------------------------------------------
qSlicerUltrasoundSegmentationFooBarWidgetPrivate
::qSlicerUltrasoundSegmentationFooBarWidgetPrivate(
  qSlicerUltrasoundSegmentationFooBarWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerUltrasoundSegmentationFooBarWidgetPrivate
::setupUi(qSlicerUltrasoundSegmentationFooBarWidget* widget)
{
  this->Ui_qSlicerUltrasoundSegmentationFooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerUltrasoundSegmentationFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerUltrasoundSegmentationFooBarWidget
::qSlicerUltrasoundSegmentationFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerUltrasoundSegmentationFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerUltrasoundSegmentationFooBarWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerUltrasoundSegmentationFooBarWidget
::~qSlicerUltrasoundSegmentationFooBarWidget()
{
}
