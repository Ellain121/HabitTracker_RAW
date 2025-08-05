#include "RegexInputDialog.hpp"

#include <QPushButton>
#include <QRegularExpressionMatch>
#include <QVBoxLayout>

RegexInputDialog::RegexInputDialog(QWidget* parent)
	: QDialog{parent}
{
	this->setAttribute(Qt::WA_QuitOnClose, false);

	QVBoxLayout* boxLayout = new QVBoxLayout{this};
	mpLabel = new QLabel{this};

	mRegexp = QRegularExpression{".*"};
	//	mRegexp.setPatternOptions(QRegularExpression::DefaultWildcardConversion);
	mpRegexpValidator = new QRegularExpressionValidator{mRegexp};

	mpLineEdit = new QLineEdit{this};
	mpLineEdit->setValidator(mpRegexpValidator);

	connect(mpLineEdit, &QLineEdit::textChanged, this, &RegexInputDialog::checkValid);

	mpButtonBox = new QDialogButtonBox(
		QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	connect(mpButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(mpButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

	boxLayout->addWidget(mpLabel);
	boxLayout->addWidget(mpLineEdit);
	boxLayout->addWidget(mpButtonBox);
}

void RegexInputDialog::setTitle(const QString& title)
{
	setWindowTitle(title);
}

void RegexInputDialog::setLabelText(const QString& label)
{
	mpLabel->setText(label);
}

void RegexInputDialog::setText(const QString& text)
{
	mpLineEdit->setText(text);
}

void RegexInputDialog::setRegExp(const QRegularExpression& regExp)
{
	mpRegexpValidator->setRegularExpression(regExp);
	checkValid(mpLineEdit->text());
}

QString RegexInputDialog::getLabelText() const
{
	return mpLabel->text();
}

QString RegexInputDialog::getText() const
{
	return mpLineEdit->text();
}

QString RegexInputDialog::getText(QWidget* parent, const QString& title,
	const QString& label, const QString& text, const QRegularExpression& regexp, bool* ok)
{
	RegexInputDialog* regexInputDialog = new RegexInputDialog(parent);
	regexInputDialog->setTitle(title);
	regexInputDialog->setLabelText(label);
	regexInputDialog->setText(text);
	regexInputDialog->setRegExp(regexp);
	*ok = regexInputDialog->exec() == QDialog::Accepted;

	if (*ok)
		return regexInputDialog->getText();
	else
		return QString();
}

void RegexInputDialog::checkValid(QString text)
{
	int	 pos = 0;
	bool valid =
		mpRegexpValidator->validate(text, pos) == QRegularExpressionValidator::Acceptable;
	mpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
}
