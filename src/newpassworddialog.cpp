// Copyright (C) 2013 July IGHOR.
// I want to create trading application that can be configured for any rule and strategy.
// If you want to help me please Donate: 1d6iMwjjNo8ZGYeJBZKXgcgVk9o7fXcjc
// For any questions please use contact form https://sourceforge.net/projects/bitcointrader/
// Or send e-mail directly to julyighor@gmail.com
//
// You may use, distribute and copy the Qt Bitcion Trader under the terms of
// GNU General Public License version 3

#include "newpassworddialog.h"
#include "main.h"
#include <QDesktopServices>
#include <QUrl>
#include <QFile>
#include <QMessageBox>
#include <QSettings>

NewPasswordDialog::NewPasswordDialog()
	: QDialog()
{
	ui.setupUi(this);
	setWindowTitle(windowTitle()+" v"+baseValues.appVerStr);
	ui.okButton->setEnabled(false);
	QSize minSizeHint=minimumSizeHint();
	if(mainWindow.isValidSize(&minSizeHint))setFixedSize(minimumSizeHint());
	setWindowFlags(Qt::WindowCloseButtonHint);

	julyTranslator.translateUi(this);

	if(QLocale().name().startsWith("zh"))ui.exchangeComboBox->setCurrentIndex(3);
	else
	exchangeChanged(ui.exchangeComboBox->currentText());
}

NewPasswordDialog::~NewPasswordDialog()
{

}

void NewPasswordDialog::exchangeChanged(QString name)
{
	ui.groupBoxApiKeyAndSecret->setTitle(julyTr("API_KEY_AND_SECRET","%1 API key and Secret").arg(name));
	switch(ui.exchangeComboBox->currentIndex())
	{
	case 2:
		ui.labelClientID->setVisible(true);
		ui.clientIdLine->setVisible(true);
		ui.clearClientIdLine->setVisible(true);
		break;
	default:
		ui.labelClientID->setVisible(false);
		ui.clientIdLine->setVisible(false);
		ui.clearClientIdLine->setVisible(false);
		break;
	}
}

QString NewPasswordDialog::getPassword()
{
	return ui.passwordEdit->text();
}

QString NewPasswordDialog::getRestSign()
{
	return ui.restSignLine->text().replace("\n","").replace("\r","").replace("\t","");
}

QString NewPasswordDialog::getRestKey()
{
	if(ui.exchangeComboBox->currentIndex()==2)
		return ui.clientIdLine->text().replace("\n","").replace("\r","").replace("\t","")+":"+ui.restKeyLine->text().replace("\n","").replace("\r","").replace("\t","");//Bitstamp
	return ui.restKeyLine->text().replace("\n","").replace("\r","").replace("\t","");
}

void NewPasswordDialog::getApiKeySecretButton()
{
	switch(ui.exchangeComboBox->currentIndex())
	{
	case 0:	QDesktopServices::openUrl(QUrl("https://www.mtgox.com/security")); break;
	case 1: QDesktopServices::openUrl(QUrl("https://btc-e.com/profile#api_keys")); break;
	case 2: QDesktopServices::openUrl(QUrl("https://www.bitstamp.net/account/security/api/")); break;
	case 3: QDesktopServices::openUrl(QUrl("https://www.btcchina.com/account/apikeys")); break;
	default: break;
	}
}

int NewPasswordDialog::getExchangeId()
{
	return ui.exchangeComboBox->currentIndex();
}

void NewPasswordDialog::checkToEnableButton()
{
	if(ui.passwordEdit->text()!=ui.confirmEdit->text()){ui.confirmLabel->setStyleSheet("color: red;");ui.okButton->setEnabled(false);return;}
	else ui.confirmLabel->setStyleSheet("");

	QString profileName=ui.profileNameEdit->text();
	if(!profileName.isEmpty())
	{
		static QString allowedNameChars="()+,-.;=@[]^_`{}~ ";
		QString allowedPName;
		for(int n=0;n<profileName.length();n++)
			if(profileName.at(n).isLetterOrNumber()||allowedNameChars.contains(profileName.at(n)))
				allowedPName.append(profileName.at(n));

		if(profileName!=allowedPName)ui.profileNameEdit->setText(allowedPName);
	}
	if(profileName.isEmpty())
	{
		ui.okButton->setEnabled(false);
		return;
	}

	if(ui.restSignLine->text().isEmpty()||ui.restKeyLine->text().isEmpty()||ui.clientIdLine->isVisible()&&ui.clientIdLine->text().isEmpty())
		{ui.okButton->setEnabled(false);return;}

	ui.okButton->setEnabled(true);
}

bool NewPasswordDialog::isValidPassword()
{
	QString pass=ui.passwordEdit->text();
	if(pass.length()<8)return false;
	if(pass!=ui.confirmEdit->text())return false;
	ui.confirmLabel->setStyleSheet("");

	static QString allowedPassChars="!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";
	bool isValidPassword=pass.length()>14;
	if(!isValidPassword)
	{
		bool containsLetter=false;
		bool containsDigit=false;
		bool containsSpec=false;
		bool containsUpCase=false;
		bool containsDownCase=false;
		for(int n=0;n<pass.length();n++)
		{
			if(!containsLetter&&pass.at(n).isLetter())containsLetter=true;
			if(!containsDigit&&pass.at(n).isDigit())containsDigit=true;
			if(!containsSpec&&allowedPassChars.contains(pass.at(n)))containsSpec=true;
			if(!containsUpCase&&pass.at(n).isLetter()&&pass.at(n).isUpper())containsUpCase=true;
			if(!containsDownCase&&pass.at(n).isLetter()&&pass.at(n).isLower())containsDownCase=true;
			if(containsLetter&&containsDigit&&containsSpec||containsLetter&&containsDigit&&containsUpCase&&containsDownCase)
			{isValidPassword=true;break;}
		}
	}
	return isValidPassword;
}

void NewPasswordDialog::updateIniFileName()
{
	if(ui.profileNameEdit->text().isEmpty())
	baseValues.iniFileName=appDataDir+"QtBitcoinTrader.ini";
	else
	{
		baseValues.iniFileName=ui.exchangeComboBox->currentText()+"_"+ui.profileNameEdit->text().toAscii()+".ini";
		baseValues.iniFileName.replace(' ','_');
		baseValues.iniFileName.prepend(appDataDir);
	}

	QSettings settings(appDataDir+"/QtBitcoinTrader.cfg",QSettings::IniFormat);
	settings.setValue("LastProfile",baseValues.iniFileName);
}

QString NewPasswordDialog::selectedProfileName()
{
	if(ui.profileNameEdit->text().isEmpty())return "Default Profile";
	return ui.profileNameEdit->text();
}

void NewPasswordDialog::okPressed()
{
	if(isValidPassword())accept();
	else
		QMessageBox::warning(this,"Qt Bitcoin Trader",julyTranslator.translateLabel("TR00100","Your password must be at least 8 characters and contain letters, digits, and special characters."));
}