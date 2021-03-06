/**
 * \file   parametercontrol.h
 * \author Tim Molloy
 *
 * $Author$
 * $Date$
 * $Rev$
 * $Id$
 *
 * Queensland University of Technology
 *
 * \section DESCRIPTION
 * Definition of the ParameterControl Widget.
 */

#ifndef PARAMETERCONTROL_H
#define PARAMETERCONTROL_H

#include <QWidget>
#include "state.h"

namespace Ui {
    class ParameterControl;
}

class ParameterControl : public QWidget {
    Q_OBJECT
public:
    ParameterControl(QWidget *parent = 0);
    ~ParameterControl();

signals:
    void sendParameters(loop_parameters_t loopConfig);

public slots:
    void SetLoopParameters(const loop_parameters_t* const srcParameters);

protected:
    void changeEvent(QEvent *e);
    QSize SizeHint() const;
    void resizeEvent (QResizeEvent* e);

private:
    Ui::ParameterControl *ui;

private slots:
    /** @name Command Buttons */
    void on_loadBtn_clicked();
    void on_SaveBtn_clicked();
    void on_SendBtn_clicked();
};

#endif // PARAMETERCONTROL_H
