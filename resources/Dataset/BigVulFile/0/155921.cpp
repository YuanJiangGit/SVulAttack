void PeopleHandler::DisplaySpinner() {
  configuring_sync_ = true;

  const int kTimeoutSec = 30;
  DCHECK(!engine_start_timer_);
  engine_start_timer_.reset(new base::OneShotTimer());
  engine_start_timer_->Start(FROM_HERE,
                             base::TimeDelta::FromSeconds(kTimeoutSec), this,
                             &PeopleHandler::DisplayTimeout);

  FireWebUIListener("page-status-changed", base::Value(kSpinnerPageStatus));
}
