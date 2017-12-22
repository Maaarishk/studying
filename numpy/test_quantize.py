import bisect
import random

import pytest
import numpy as np

from quantize import quantize


def quantize_slow(x, n):
    values = sorted(list(x))
    boundaries = [values[len(x) * p // n] for p in range(n)]

    quantized = [bisect.bisect_left(boundaries, x[i]) for i in range(len(x))]
    return np.array(boundaries), np.array(quantized)


def test_quantize_on_sample_data():
    boundaries, quantized = quantize(np.array(range(10), dtype=np.float), 10)
    assert np.all(list(range(10)) == boundaries)
    assert np.all(list(range(10)) == quantized)


def test_quantize_unsorted_input():
    x = list(range(10))
    random.shuffle(x)
    x = np.array(x, dtype=np.float)
    boundaries, quantized = quantize(x, 10)
    assert np.all(list(range(10)) == boundaries)
    assert boundaries.dtype == np.float


def test_quantize_float32_specialization():
    boundaries, quantized = quantize(np.array(range(10), dtype=np.float32), 10)
    assert np.all(list(range(10)) == boundaries)
    assert np.all(list(range(10)) == quantized)
    assert boundaries.dtype == np.float32


def test_quantize_edge_cases():
    boundaries, quantized = quantize(np.array([0] * 10, dtype=np.float), 10)
    assert np.all(np.zeros(10) == boundaries)
    assert np.all(np.zeros(10) == quantized)


def test_type_checks():
    with pytest.raises(TypeError):
        quantize([1, 2, 3], 2)

    with pytest.raises(TypeError):
        quantize(np.array([1, 2, 3]), 2)


if __name__ == "__main__":
    pass
