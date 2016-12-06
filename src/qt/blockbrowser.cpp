#include "blockbrowser.h"
#include "ui_blockbrowser.h"
#include "main.h"
#include "init.h"
#include "wallet.h"
#include "base58.h"
#include "clientmodel.h"
#include "bitcoinrpc.h"
#include "transactionrecord.h"

#include <sstream>
#include <string>

BlockBrowser::BlockBrowser(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::BlockBrowser)
{
	ui->setupUi(this);

	setFixedSize(400, 420);

	connect(ui->blockButton, SIGNAL(pressed()), this, SLOT(blockClicked()));
	connect(ui->txButton, SIGNAL(pressed()), this, SLOT(txClicked()));
}

void BlockBrowser::updateExplorerBlock()
{
	int height = ui->heightBox->value();
	if (height < 0 || height > nBestHeight) {
		std::string invalid = "Block number out of range";
		std::string empty = " ";
		QString QInvalid = QString::fromUtf8(invalid.c_str());
		QString QEmpty = QString::fromUtf8(empty.c_str());

		ui->heightValue->setText(QInvalid);
		ui->timeStampValue->setText(QEmpty);
		ui->hashValue->setText(QEmpty);
		ui->diffValue->setText(QEmpty);
		ui->confirmValue->setText(QEmpty);
		ui->merkleValue->setText(QEmpty);
		ui->nonceValue->setText(QEmpty);
	}
	else {
		CBlockIndex* pblockindex = FindBlockByHeight(height);
		std::string strHash = pblockindex->phashBlock->GetHex();

		uint256 hash(strHash);
		CBlock block;
		block.ReadFromDisk(pblockindex);

		CMerkleTx txGen(block.vtx[0]);
		txGen.SetMerkleBranch(&block);
		int confirmations = (int)txGen.GetDepthInMainChain();
		std::string merkle = block.hashMerkleRoot.GetHex();
		int64 blockTimeStamp = block.GetBlockTime();
		int nonce = (int)block.nNonce;
		double diff = getBlockDiff(pblockindex);

		QString QHeight = QString::number(height);
		QString QTimeStamp = QString::fromUtf8(DateTimeStrFormat("%Y-%m-%d %H:%M:%S",
			blockTimeStamp).c_str());
		QString QHash = QString::fromUtf8(strHash.c_str());
		QString QDiff = QString::number(diff, 'f', 6);
		QString QConfirm = QString::number(confirmations);
		QString QMerkle = QString::fromUtf8(merkle.c_str());
		QString QNonce = QString::number(nonce);
		
		ui->heightValue->setText(QHeight);
		ui->timeStampValue->setText(QTimeStamp);
		ui->hashValue->setText(QHash);
		ui->diffValue->setText(QDiff);
		ui->confirmValue->setText(QConfirm);
		ui->merkleValue->setText(QMerkle);
		ui->nonceValue->setText(QNonce);
	}

	ui->heightLabel->show();
	ui->heightValue->show();
	ui->timeStampLabel->show();
	ui->timeStampValue->show();
	ui->hashLabel->show();
	ui->hashValue->show();
	ui->diffLabel->show();
	ui->diffValue->show();
	ui->confirmLabel->show();
	ui->confirmValue->show();
	ui->merkleLabel->show();
	ui->merkleValue->show();
	ui->nonceLabel->show();
	ui->nonceValue->show();
}

void BlockBrowser::updateExplorerTx()
{
	std::string txid = ui->txBox->text().toUtf8().constData();
	uint256 hash;
	hash.SetHex(txid);

	if (!pwalletMain->mapWallet.count(hash)) {
		txid = "Invalid or non-wallet transaction id";
		QString QID = QString::fromUtf8(txid.c_str());
		std::string empty = " ";
		QString QEmpty = QString::fromUtf8(empty.c_str());

		ui->txID->setText(QID);
		ui->txTimeValue->setText(QEmpty);
		ui->amountValue->setText(QEmpty);
		ui->txConfirmValue->setText(QEmpty);
		ui->feesValue->setText(QEmpty);
	}
	else {
		const CWalletTx& wtx = pwalletMain->mapWallet[hash];

		int64 nCredit = wtx.GetCredit();
		int64 nDebit = wtx.GetDebit();
		int64 nNet = nCredit - nDebit;
		int64 nFee = (wtx.IsFromMe() ? wtx.GetValueOut() - nDebit : 0);

		double amount = (double)(nNet - nFee) / (double)COIN;
		double fees = (double)nFee / (double)COIN;
		int confirms = wtx.GetDepthInMainChain();
		int64 txTimeStamp = wtx.GetTxTime();

		QString QID = QString::fromUtf8(txid.c_str());
		QString QTimeStamp = QString::fromUtf8(DateTimeStrFormat("%Y-%m-%d %H:%M:%S",
			txTimeStamp).c_str());
		QString QAmount = QString::number(amount, 'f', 6);
		QString QConfirms = QString::number(confirms);
		QString QFees = QString::number(fees, 'f', 6);

		ui->txID->setText(QID);
		ui->txTimeValue->setText(QTimeStamp);
		ui->amountValue->setText(QAmount + " KURRENT");
		ui->txConfirmValue->setText(QConfirms);
		ui->feesValue->setText(QFees + " KURRENT");
	}

	ui->txID->show();
	ui->txLabel->show();
	ui->txTimeLabel->show();
	ui->txTimeValue->show();
	ui->amountLabel->show();
	ui->amountValue->show();
	ui->txConfirmLabel->show();
	ui->txConfirmValue->show();
	ui->feesLabel->show();
	ui->feesValue->show();
}


void BlockBrowser::txClicked()
{
	updateExplorerTx();
}

void BlockBrowser::blockClicked()
{
	updateExplorerBlock();
}

void BlockBrowser::setClientModel(ClientModel *model)
{
	this->model = model;
}

BlockBrowser::~BlockBrowser()
{
	delete ui;
}

double getBlockDiff(CBlockIndex* blockindex)
{
    int nShift = (blockindex->nBits >> 24) & 0xff;

    double dDiff =
        (double)0x0000ffff / (double)(blockindex->nBits & 0x00ffffff);

    while (nShift < 29)
    {
        dDiff *= 256.0;
        nShift++;
    }
    while (nShift > 29)
    {
        dDiff /= 256.0;
        nShift--;
    }

    return dDiff;
}

