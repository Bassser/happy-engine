#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

namespace hs {
class GameWidget;
class MaterialEditor;
class OptionsForm;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void closeEvent(QCloseEvent *event);

    GameWidget* getGameWidget() const;

private slots:
    void openMaterialEditor();
    void openOptionsForm();

private:
    Ui::MainWindow* m_UI;

    OptionsForm* m_OptionsForm;
    MaterialEditor* m_MaterialEditor;
};

}
#endif // MAINWINDOW_H
