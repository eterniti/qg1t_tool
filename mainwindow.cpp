#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include "debug.h"

#define PROGRAM_NAME    "qg1t_tool"
#define PROGRAM_VERSION "0.3"

#define MAX_INFO_WIDTH  300
#define MAX_WIDTH_RATIO 0.75

#define SB_TIMEOUT  5000

enum InfoRow
{
    ROW_SIZE,
    ROW_DX_FORMAT,
    ROW_G1T_FORMAT,
    ROW_MIPS,
    ROW_COUNT
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    set_debug_level(2);

    ui->setupUi(this);
    setWindowTitle(QString("%1 %2").arg(PROGRAM_NAME).arg(PROGRAM_VERSION));

    ui->centralWidget->setBackgroundRole(QPalette::Mid);

    ui->statusBar->showMessage("Use File->Open or drag a .g1t file in the window");

    extractButton = new QPushButton("Extract", this);
    extractAllButton = new QPushButton("Extract all", this);
    replaceBrowseButton = new QPushButton("Replace (browse)", this);
    replaceButton = new QPushButton("Replace", this);
    fileEdit = new QLineEdit(this);
    texSelectBox = new QComboBox(this);

    ui->statusBar->addPermanentWidget(extractButton);
    ui->statusBar->addPermanentWidget(extractAllButton);
    ui->statusBar->addPermanentWidget(replaceBrowseButton);
    ui->statusBar->addPermanentWidget(replaceButton);
    ui->statusBar->addPermanentWidget(fileEdit);
    ui->statusBar->addPermanentWidget(texSelectBox);

    //origImage = QImage(ui->imageFrame->width(), ui->imageFrame->height(), QImage::Format_ARGB32);
    origImage = QImage(1, 1, QImage::Format_ARGB32);
    origImage.fill(QColor(255, 0, 255));

    // Disable visibility by default
    ui->imageFrame->setVisible(false);
    ui->infoTreeWidget->setVisible(false);

    extractButton->setVisible(false);
    extractAllButton->setVisible(false);
    replaceBrowseButton->setVisible(false);
    replaceButton->setVisible(false);
    fileEdit->setVisible(false);
    texSelectBox->setVisible(false);

    g1t = nullptr;

    if (qApp->arguments().size() >= 2)
    {
        argFile = qApp->arguments()[1];
    }

    showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::GetG1TDir()
{
    QString dir = g1tPath;

    if (!dir.isEmpty())
    {
        int ls1 = dir.lastIndexOf('/');
        int ls2 = dir.lastIndexOf('\\');

        if (ls1 > ls2)
        {
            dir = dir.mid(0, ls1+1);
        }
        else if (ls2 > 0)
        {
            dir = dir.mid(0, ls2+1);
        }
    }

    return dir;
}

void MainWindow::SetImage(QImage &image, const uint32_t *raw, uint32_t width, uint32_t height, bool alpha)
{
    if (alpha)
        image = QImage((int)width, (int)height, QImage::Format_ARGB32);
    else
        image = QImage((int)width, (int)height, QImage::Format_RGB32);

    image.fill(QColor(255, 0, 255));

    for(uint32_t y = 0; y < height; y++)
    {
        uint8_t *lineOut = image.scanLine((int)y);
        const uint8_t *lineIn = (const uint8_t *)(raw + (y*width));

        for(uint32_t x = 0; x < width; x++)
        {
            lineOut[0] = lineIn[0];
            lineOut[1] = lineIn[1];
            lineOut[2] = lineIn[2];

            if (alpha)
                lineOut[3] = lineIn[3];
            else
                lineOut[3] = 0xFF;

            lineOut += 4;
            lineIn += 4;
        }
    }
}

bool MainWindow::ShowTexture(size_t idx)
{
    if (!g1t)
        return false;

    uint32_t *dec;
    bool alpha;
    uint32_t w, h;

    if (arr_textures.size() > 0)
    {
        if (idx >= arr_textures.size())
            return false;

        dec = G1tFile::Decode(arr_textures[idx], &alpha, true);
        w = arr_textures[idx].width;
        h = arr_textures[idx].height;
    }
    else
    {
        dec = g1t->Decode(idx, &alpha, true);
        w = (*g1t)[idx].width;
        h = (*g1t)[idx].height;
    }

    if (!dec)
        return false;

    SetImage(origImage, dec, w, h, alpha);
    UpdateImage();

    return true;
}

QString MainWindow::GetFormatAlias(int dx_format)
{
    QString alias;

    switch (dx_format)
    {
        case DXGI_FORMAT_B8G8R8A8_UNORM:
            alias = "ARGB";
        break;

        case DXGI_FORMAT_BC1_UNORM:
            alias = "DXT1";
        break;

        case DXGI_FORMAT_BC2_UNORM:
            alias = "DXT3";
        break;

        case DXGI_FORMAT_BC3_UNORM:
            alias = "DXT5";
        break;

        case DXGI_FORMAT_BC4_UNORM:
            alias = "ATI1";
        break;
    }

    return alias;
}

void MainWindow::UpdateTextureInfo(size_t idx)
{
    const G1tTexture *tex;

    if (arr_textures.size() > 0)
    {
        if (idx >= arr_textures.size())
            return;

        tex = &arr_textures[idx];
    }
    else
    {
        if (!g1t || idx >= g1t->GetNumTextures())
            return;

        tex = &(*g1t)[idx];
    }

    int dx_format;
    QString alias;

    for (int i = 0; i < ui->infoTreeWidget->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = ui->infoTreeWidget->topLevelItem(i);

        switch (i)
        {
            case ROW_SIZE:
                item->setText(1, QString("%1x%2").arg(tex->width).arg(tex->height));
            break;

            case ROW_DX_FORMAT:

                dx_format = G1tFile::G1tToDdsFormat(tex->format);
                alias = GetFormatAlias(dx_format);

                if (alias.isEmpty())
                {
                    item->setText(1, Utils::StdStringToQString(DdsFile::GetFormatName(dx_format)));
                }
                else
                {
                    item->setText(1, QString("%1 (alias %2)").arg(Utils::StdStringToQString(DdsFile::GetFormatName(dx_format))).arg(alias));
                }

            break;

            case ROW_G1T_FORMAT:
                item->setText(1, QString("0x%1").arg((int)(tex->format), 2, 16, QChar('0')));
            break;

            case ROW_MIPS:
                if (tex->mips <= 1)
                    item->setText(1, "No");
                else
                    item->setText(1, QString("Yes (%1)").arg(tex->mips));
            break;
        }

        item->setToolTip(0, item->text(1));
        item->setToolTip(1, item->text(1));
    }
}

bool MainWindow::LoadG1t(const QString &path)
{
    g1tPath = path;

    if (g1t)
    {
        delete g1t;
        g1t = nullptr;
    }

    arr_textures.clear();

    disconnect(extractButton, SIGNAL(clicked()), this, SLOT(onExtractTexture()));
    disconnect(extractAllButton, SIGNAL(clicked()), this, SLOT(onExtractAllTextures()));
    disconnect(replaceBrowseButton, SIGNAL(clicked()), this, SLOT(onReplaceBrowseTexture()));
    disconnect(replaceButton, SIGNAL(clicked()), this, SLOT(onReplaceTexture()));
    disconnect(texSelectBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onTexSelectionChanged(int)));

    ui->statusBar->clearMessage();
    ui->imageFrame->setVisible(false);
    ui->infoTreeWidget->setVisible(false);
    extractButton->setVisible(false);
    extractAllButton->setVisible(false);
    replaceBrowseButton->setVisible(false);
    replaceButton->setVisible(false);
    fileEdit->clear();
    fileEdit->setVisible(false);
    texSelectBox->clear();
    texSelectBox->setVisible(false);

    g1t = new G1tFile();
    if (!g1t->LoadFromFile(Utils::QStringToStdString(path, true)))
    {
        ui->statusBar->showMessage("Failed to load .g1t file.");
        delete g1t; g1t = nullptr;
        return false;
    }

    if (g1t->GetNumTextures() == 0)
    {
        ui->statusBar->showMessage("That .g1t doesn't have textures (?)");
        delete g1t; g1t = nullptr;
        return false;
    }

    if (g1t->GetNumTextures() == 1 && g1t->IsArrayTexture(0))
    {
        if (!g1t->DecomposeArrayTexture(0, arr_textures, false, true))
        {
            ui->statusBar->showMessage("Error decomposing an array texture.");

            delete g1t; g1t = nullptr;
            return false;
        }
    }

    ui->imageFrame->setVisible(true);
    ShowTexture(0);

    ui->infoTreeWidget->setVisible(true);
    UpdateTextureInfo(0);

    extractButton->setVisible(true);
    replaceBrowseButton->setVisible(true);
    replaceButton->setVisible(true);
    fileEdit->setVisible(true);

    connect(extractButton, SIGNAL(clicked()), this, SLOT(onExtractTexture()));
    connect(replaceBrowseButton, SIGNAL(clicked()), this, SLOT(onReplaceBrowseTexture()));
    connect(replaceButton, SIGNAL(clicked()), this, SLOT(onReplaceTexture()));

    if (arr_textures.size() > 0 || g1t->GetNumTextures() > 1)
    {
        extractAllButton->setVisible(true);
        texSelectBox->setVisible(true);

        size_t n = (arr_textures.size() > 0) ? arr_textures.size() : g1t->GetNumTextures();

        for (size_t i = 0; i < n; i++)
        {
            if (arr_textures.size() > 0)
                texSelectBox->addItem(QString("Arr %1").arg(i));
            else
                texSelectBox->addItem(QString("Tex %1").arg(i));
        }

        connect(extractAllButton, SIGNAL(clicked()), this, SLOT(onExtractAllTextures()));
        connect(texSelectBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onTexSelectionChanged(int)));
    }

    ui->statusBar->showMessage("G1T loaded succesfully.", SB_TIMEOUT);
    return true;
}

void MainWindow::UpdateImage()
{
    if (!argFile.isEmpty())
    {
        QString file = argFile;
        argFile.clear();

        if (LoadG1t(file))
            return;
    }

    QPalette p = QPalette();

    QSize size = ui->centralWidget->size();
    bool centerY = false;

    if (origImage.width() > origImage.height())
    {
        QSize imgSize = origImage.size();
        imgSize.scale(size, Qt::KeepAspectRatio);

        double win_ratio = (double)ui->centralWidget->width() / (double) imgSize.width();

        if (win_ratio > MAX_WIDTH_RATIO)
        {
            double ratio = (double)origImage.height() / (double)origImage.width();
            int w = (int) (MAX_WIDTH_RATIO * (double)ui->centralWidget->width());
            int h = (int) ((double)w * ratio);
            resizedImage = origImage.scaled(w, h, Qt::KeepAspectRatio);
            centerY = true;
        }
        else
        {
            resizedImage = origImage.scaled(size, Qt::KeepAspectRatio);
        }
    }
    else
    {
        resizedImage = origImage.scaled(size, Qt::KeepAspectRatio);
    }

    //printf("%d %d to %d %d  %d %d\n", origImage.width(), origImage.height(), resizedImage.width(), resizedImage.height(), size.width(), size.height());

    ui->imageFrame->resize(resizedImage.size());
    p.setBrush(QPalette::Mid, resizedImage);
    ui->imageFrame->setPalette(p);

    int x = (ui->centralWidget->width() / 2) - (resizedImage.width() / 2);
    int y = 0;

    if (centerY)
    {
        y = (ui->centralWidget->height() / 2) - (resizedImage.height() / 2);
    }

    ui->imageFrame->move(x, y);
}

bool MainWindow::ExtractTexture(const G1tTexture &tex, const QString &path)
{
    DdsFile *dds = G1tFile::ToDDS(tex);
    if (!dds)
    {
        DPRINTF("Failed to convert to dds.\n");
        ui->statusBar->showMessage("Failed to convert to dds.");
        return false;
    }

    bool ret = dds->SaveToFile(Utils::QStringToStdString(path));
    delete dds;

    if (!ret)
    {
        ui->statusBar->showMessage("Saving dds failed.");
        return false;
    }

    return true;
}

bool MainWindow::ReplaceTexture(G1tTexture &tex, const QString &path, bool array, QString &format_change)
{
    G1tTexture copy = tex;
    DdsFile dds;

    if (!dds.LoadFromFile(Utils::QStringToStdString(path)))
    {
        ui->statusBar->showMessage("Failed to load dds file.");
        return false;
    }

    uint8_t fmt, prev_fmt;

    if (!G1tFile::FromDDS(copy, dds, &fmt, &prev_fmt))
    {
        DPRINTF("Failed to replace texture.\n");
        ui->statusBar->showMessage("Failed to replace texture.");
    }

    format_change = "";

    if (fmt != prev_fmt)
    {
        int dds_fmt = G1tFile::G1tToDdsFormat(fmt);
        int dds_prev_fmt =  G1tFile::G1tToDdsFormat(prev_fmt);

        if (dds_fmt != dds_prev_fmt)
        {
            format_change = QString("Notice: format changed from %1 to %2.").
                    arg(Utils::StdStringToQString(DdsFile::GetFormatName(dds_prev_fmt))).
                    arg(Utils::StdStringToQString(DdsFile::GetFormatName(dds_fmt)));
        }
    }

    if (array)
    {
        if (copy.width != tex.width || copy.height != tex.height)
        {
            DPRINTF("Error: in an array textures, all images dimensions must match (Mismatch: %ux%u != %ux%u)\n"
                    "To solve this error, you can proceed in two different ways:\n\n"
                    "Method 1: make the image you were using the same size as the other image(s) in this .g1t.\n"
                    "Method 2: if you really require this size, change the other image(s) size to match this one, "
                    "and then use the command line g1t_replace to replace all of them at once.", copy.width, copy.height, tex.width, tex.height);

            ui->statusBar->showMessage("Failed to replace texture (array texture specific error)");
            return false;
        }

        if (copy.format != tex.format)
        {
            DPRINTF("Error: in an array textures, all images formats must match. (Mismatch: %s != %s).\n"
                    "To solve this error, you can proceed in two different ways:\n\n"
                    "Method 1: make the image you were using the same format as the other image(s) in this .g1t.\n"
                    "Method 2: if you really require this format or can't use other format, change the other image(s) format to match this one, "
                    "and then use the command line g1t_replace to replace all of them at once.",
                    DdsFile::GetFormatName(G1tFile::G1tToDdsFormat(copy.format)).c_str(),
                    DdsFile::GetFormatName(G1tFile::G1tToDdsFormat(tex.format)).c_str());

            ui->statusBar->showMessage("Failed to replace texture (array texture specific error)");
            return false;
        }
    }
    else
    {
        if (copy.mips > tex.mips)
        {
            if (!G1tFile::ReduceMipsLevel(copy, tex.mips))
            {
                DPRINTF("Internal error: failed to reduce mips levels.\n");
                return false;
            }
        }
    }

    tex = copy; // commit changes

    if (array)
    {
        uint8_t lower_mips = arr_textures.front().mips;
        bool patch_needed = false;

        for (size_t i = 1; i < arr_textures.size(); i++)
        {
            const G1tTexture &tex = arr_textures[i];

            if (tex.mips != lower_mips)
            {
                patch_needed = true;

                if (tex.mips < lower_mips)
                    lower_mips = tex.mips;
            }
        }

        if (patch_needed)
        {
            for (G1tTexture &tex: arr_textures)
            {
                if (tex.mips != lower_mips && !G1tFile::ReduceMipsLevel(tex, lower_mips))
                {
                    DPRINTF("Internal error while reducing mips levels to make array images match.\n");
                    ui->statusBar->showMessage("Internal error.");
                    return false;
                }
            }
        }
    }

    return true;
}

bool MainWindow::ReplaceCurrentTexture(const QString &path)
{
    if (!g1t)
        return false;

    G1tTexture *tex;
    size_t tex_index;

    if (arr_textures.size() > 0)
    {
        int index = texSelectBox->currentIndex();
        if (index < 0 || index >= (int)arr_textures.size())
            return false;

        tex = &arr_textures[(size_t)index];
        tex_index = (size_t)index;
    }
    else if (g1t->GetNumTextures() > 1)
    {
        int index = texSelectBox->currentIndex();
        if (index < 0 || index >= (int)g1t->GetNumTextures())
            return false;

        tex = &(*g1t)[(size_t)index];
        tex_index = (size_t)index;
    }
    else
    {
        tex = &(*g1t)[0];
        tex_index = 0;
    }

    QString format_change;

    if (!ReplaceTexture(*tex, path, arr_textures.size() > 0, format_change))
        return false;

    if (arr_textures.size() > 0)
    {
        if (!g1t->ComposeArrayTexture(0, arr_textures, true))
        {
            ui->statusBar->showMessage("Error recomposing array texture.");
            return false;
        }
    }

    if (!g1t->SaveToFile(Utils::QStringToStdString(g1tPath)))
    {
        ui->statusBar->showMessage("Error updating .g1t.\n");
        return false;
    }

    ShowTexture(tex_index);
    UpdateTextureInfo(tex_index);

    ui->statusBar->showMessage("Texture succesfully replaced. " + format_change, SB_TIMEOUT);
    return true;
}

void MainWindow::resizeEvent(QResizeEvent *re)
{
    const QSize &oldSize = re->oldSize();
    //const QSize &newSize = re->size();

    if (oldSize.width() < 0 || oldSize.height() < 0)
    {
        origCentralWidgetSize = ui->centralWidget->size();
        return;
    }

    UpdateImage();

    int infoX = ui->imageFrame->x() + ui->imageFrame->width();
    int infoWidth = ui->centralWidget->width() - infoX;

    if (infoWidth > MAX_INFO_WIDTH)
    {
        infoWidth = MAX_INFO_WIDTH;
        infoX = ui->centralWidget->width() - infoWidth;
    }

    ui->infoTreeWidget->move(infoX, 0);
    ui->infoTreeWidget->resize(infoWidth, ui->centralWidget->height());
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData *mime = event->mimeData();
    if (mime->hasText())
    {
        QString text = mime->text();

        if (text.startsWith("file:///") && (text.endsWith(".dds", Qt::CaseInsensitive) || text.endsWith(".g1t", Qt::CaseInsensitive)))
            event->accept();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mime = event->mimeData();
    if (mime->hasText())
    {
        if (event->proposedAction() != Qt::CopyAction)
            return;

        QString text = mime->text();

        if (!text.startsWith("file:///"))
            return;

        if (text.endsWith(".g1t", Qt::CaseInsensitive))
        {
             if (LoadG1t(text.mid(8)))
             {
                 event->acceptProposedAction();
                 event->accept();
             }
        }
        else if (text.endsWith(".dds", Qt::CaseInsensitive))
        {
            if (!g1t)
                return;

            fileEdit->setText(text.mid(8));
            onReplaceTexture();
            event->acceptProposedAction();
            event->accept();
        }
    }
}

void MainWindow::on_actionOpen_triggered()
{
    QString dir = GetG1TDir();

    QString file = QFileDialog::getOpenFileName(this, "Open .g1t file", dir, "G1T Files (*.g1t)");
    if (file.isEmpty())
        return;

    LoadG1t(file);
}

void MainWindow::on_actionExit_triggered()
{
    qApp->exit();
}

void MainWindow::on_actionAbout_triggered()
{
    UPRINTF("%s %s by eternity", PROGRAM_NAME, PROGRAM_VERSION);
}

void MainWindow::onTexSelectionChanged(int index)
{
    if (!g1t || index < 0)
        return;

    ShowTexture((size_t)index);
    UpdateTextureInfo((size_t)index);
}

static QString format_to_extension(int dds_format)
{
    switch (dds_format)
    {
        case DXGI_FORMAT_B8G8R8A8_UNORM:
            return ".rgba.dds";

        case DXGI_FORMAT_BC1_UNORM:
            return ".dxt1.dds";

        case DXGI_FORMAT_BC2_UNORM:
            return ".dxt3.dds";

        case DXGI_FORMAT_BC3_UNORM:
            return ".dxt5.dds";

        case DXGI_FORMAT_BC4_UNORM:
            return ".bc4.dds";

        case DXGI_FORMAT_BC6H_UF16:
            return ".bc6h.dds";
    }

    QString name = Utils::StdStringToQString(DdsFile::GetFormatName(dds_format));
    int pos = name.lastIndexOf("_UNORM");
    if (pos > 0)
    {
        name = name.mid(0, pos);
        return "." + name.toLower() + ".dds";
    }

    return ".dds";
}

void MainWindow::onExtractTexture()
{
    if (!g1t)
        return;

    const G1tTexture *tex;
    int tex_index = -1;

    if (arr_textures.size() > 0)
    {
        int index = texSelectBox->currentIndex();
        if (index < 0 || index >= (int)arr_textures.size())
            return;

        tex = &arr_textures[(size_t)index];
        tex_index = index;
    }
    else if (g1t->GetNumTextures() > 1)
    {
        int index = texSelectBox->currentIndex();
        if (index < 0 || index >= (int)g1t->GetNumTextures())
            return;

        tex = &(*g1t)[(size_t)index];
        tex_index = index;
    }
    else
    {
        tex = &(*g1t)[0];
    }

    QString dds = g1tPath;

    int ext = dds.lastIndexOf(".g1t", -1, Qt::CaseInsensitive);
    if (ext >= 0)
    {
        dds = dds.mid(0, ext);

        if (tex_index >= 0)
            dds += QString("_%1").arg(tex_index);

        dds += format_to_extension(G1tFile::G1tToDdsFormat(tex->format));
    }

    QString file = QFileDialog::getSaveFileName(this, "Save dds file", dds, "DDS Files (*.dds)");
    if (file.isEmpty())
        return;

    if (ExtractTexture(*tex, file))
    {
        ui->statusBar->showMessage("Texture succesfully extracted.", SB_TIMEOUT);
    }
}

void MainWindow::onExtractAllTextures()
{
    if (!g1t)
        return;

    QString dir;

    dir = GetG1TDir();
    dir = QFileDialog::getExistingDirectory(this, "Choose a folder", dir);
    if (dir.isEmpty())
        return;

    if (!dir.endsWith('/') && !dir.endsWith('\\'))
        dir.push_back('/');

    size_t n = (arr_textures.size() > 0) ? arr_textures.size() : g1t->GetNumTextures();
    for (size_t i = 0; i < n; i++)
    {
        const G1tTexture *tex;
        QString file = dir;

        if (arr_textures.size() > 0)
        {
            tex = &arr_textures[i];

            QString format = format_to_extension(G1tFile::G1tToDdsFormat(tex->format));
            file += QString("Arr %1%2").arg(i).arg(format);
        }
        else
        {
            tex = &(*g1t)[i];

            QString format = format_to_extension(G1tFile::G1tToDdsFormat(tex->format));
            file += QString("Tex %1%2").arg(i).arg(format);
        }

        if (!ExtractTexture(*tex, file))
            return;
    }

    ui->statusBar->showMessage("All textures succesfully extracted.", SB_TIMEOUT);
}

void MainWindow::onReplaceBrowseTexture()
{
    QString dir = GetG1TDir();
    QString file = QFileDialog::getOpenFileName(this, "Select dds file", dir, "DDS Files (*.dds)");
    if (file.isEmpty())
        return;

    fileEdit->setText(file);
    onReplaceTexture();
}

void MainWindow::onReplaceTexture()
{
    QString file = fileEdit->text().trimmed();
    if (file.isEmpty() || !g1t)
        return;

    ReplaceCurrentTexture(file);
}

