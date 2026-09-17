import sys
from pathlib import Path
import unittest


sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from pose_filter import EmaPoseFilter


class EmaPoseFilterTest(unittest.TestCase):
    def test_first_sample_is_returned_directly(self):
        filter_ = EmaPoseFilter(alpha=0.35)
        self.assertEqual(filter_.update(0.1, 1.0), (0.1, 1.0))

    def test_second_sample_uses_ema(self):
        filter_ = EmaPoseFilter(alpha=0.5)
        filter_.update(0.0, 1.0)
        tx, tz = filter_.update(0.2, 0.6)
        self.assertAlmostEqual(tx, 0.1)
        self.assertAlmostEqual(tz, 0.8)

    def test_reset_removes_previous_state(self):
        filter_ = EmaPoseFilter()
        filter_.update(0.2, 0.8)
        filter_.reset()
        self.assertEqual(filter_.update(-0.1, 0.5), (-0.1, 0.5))

    def test_invalid_depth_is_rejected(self):
        with self.assertRaises(ValueError):
            EmaPoseFilter().update(0.0, 0.0)


if __name__ == "__main__":
    unittest.main()

