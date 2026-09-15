class EmaPoseFilter:

    def __init__(self, alpha=0.35):
        if not 0 < alpha <= 1:
            raise ValueError("Invalid Value.")

        self.alpha = alpha
        self.filtered_tx = None
        self.filtered_tz = None

    def update(self, tx, tz):
        if self.filtered_tx is None:
            self.filtered_tx = tx
            self.filtered_tz = tz
        else:
            self.filtered_tx = (
                self.alpha * tx
                + (1 - self.alpha) * self.filtered_tx
            )

            self.filtered_tz = (
                self.alpha * tz
                + (1 - self.alpha) * self.filtered_tz
            )

        return self.filtered_tx, self.filtered_tz

    def reset(self):
        self.filtered_tx = None
        self.filtered_tz = None