#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QResizeEvent>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QImage>
#include <QMimeData>

#include "DOA6/G1tFile.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;

    QSize origCentralWidgetSize;

    QPushButton *extractButton;
    QPushButton *extractAllButton;
    QPushButton *replaceBrowseButton;
    QPushButton *replaceButton;
    QLineEdit *fileEdit;
    QComboBox *texSelectBox;    

    QImage origImage;
    QImage resizedImage;

    G1tFile *g1t;
    std::vector<G1tTexture> arr_textures;

    QString argFile;
    QString g1tPath;

    QString GetG1TDir();

    void SetImage(QImage &image, const uint32_t *raw, uint32_t width, uint32_t height, bool alpha);
    bool ShowTexture(size_t idx);

    QString GetFormatAlias(int dx_format);
    void UpdateTextureInfo(size_t idx);

    bool LoadG1t(const QString &path);
    void UpdateImage();

    bool ExtractTexture(const G1tTexture &tex, const QString &path);
    bool ReplaceTexture(G1tTexture &tex, const QString &path, bool array, QString &format_change);
    bool ReplaceCurrentTexture(const QString &path);

private slots:
    void resizeEvent(QResizeEvent *re) override;    

    void on_actionOpen_triggered();
    void on_actionExit_triggered();
    void on_actionAbout_triggered();

    void onTexSelectionChanged(int index);
    void onExtractTexture();
    void onExtractAllTextures();
    void onReplaceBrowseTexture();
    void onReplaceTexture();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};

#endif // MAINWINDOW_H
