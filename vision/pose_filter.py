import math


class EmaPoseFilter:
    def __init__(self, alpha: float = 0.35):
        if not 0.0 < alpha <= 1.0:
            raise ValueError("alpha must be in the range (0, 1]")

        self.alpha = alpha
        self.filtered_tx = None
        self.filtered_tz = None

    def update(self, tx: float, tz: float) -> tuple[float, float]:
        if not math.isfinite(tx) or not math.isfinite(tz) or tz <= 0.0:
            raise ValueError("pose values must be finite and tz must be positive")

        if self.filtered_tx is None:
            self.filtered_tx = tx
            self.filtered_tz = tz
        else:
            self.filtered_tx = (
                self.alpha * tx
                + (1.0 - self.alpha) * self.filtered_tx
            )
            self.filtered_tz = (
                self.alpha * tz
                + (1.0 - self.alpha) * self.filtered_tz
            )

        return self.filtered_tx, self.filtered_tz

    def reset(self) -> None:
        self.filtered_tx = None
        self.filtered_tz = None

