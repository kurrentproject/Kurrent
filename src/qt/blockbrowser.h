#ifndef BLOCKBROWSER_H
#define BLOCKBROWSER_H

#include "clientmodel.h"
#include "wallet.h"
#include "base58.h"
#include <QWidget>

#include <QDir>
#include <QFile>
#include <QProcess>
#include <QTime>
#include <QTimer>
#include <QStringList>
#include <QMap>
#include <QSettings>
#include <QSlider>

double getBlockDiff(CBlockIndex*);

namespace Ui {
class BlockBrowser;
}
class ClientModel;

class BlockBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit BlockBrowser(QWidget *parent = 0);
    ~BlockBrowser();
    
    void setClientModel(ClientModel *model);
    
public slots:
    void blockClicked();
    void txClicked();
    void updateExplorerBlock();
	void updateExplorerTx();

private slots:

private:
    Ui::BlockBrowser *ui;
    ClientModel *model;
    
};

#endif // BLOCKBROWSER_H