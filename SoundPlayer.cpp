#include "SoundPlayer.hpp"

#include <QDebug>

std::unique_ptr<QSoundEffect> SoundPlayer::mSoundPlayer{std::make_unique<QSoundEffect>()};
bool						  SoundPlayer::mPlayerActive{false};

void SoundPlayer::initSound()
{
    SoundPlayer::mSoundPlayer->setSource(QUrl{"qrc:/success.wav"});
    SoundPlayer::mSoundPlayer->setVolume(0.75f);

    SoundPlayer::mPlayerActive = true;
}

void SoundPlayer::playSound(Type /*soundType*/)
{
    if (!SoundPlayer::mPlayerActive)
    {
        SoundPlayer::initSound();
    }

    SoundPlayer::mSoundPlayer->play();
}
