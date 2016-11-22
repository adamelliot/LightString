

template <typename TYPE>
TPoint<TYPE, 2> TRect<TYPE>::randomPoint() {
	TPoint<TYPE, 2> ret;

	ret.x = random(x * 1000, (x + width) * 1000) / 1000.0;
	ret.y = random(y * 1000, (y + height) * 1000) / 1000.0;

	return ret;
}

template <typename TYPE>
bool TCuboid<TYPE>::containsPoint(TPoint<TYPE, 3> pt) {
	if (pt.x < x || pt.y < y || pt.z < z ||
	        pt.x > (x + width) || pt.y > (y + height) || pt.z > (z + depth))
		return false;

	return true;
}

template <typename TYPE>
TPoint<TYPE, 3> TCuboid<TYPE>::boundPoint(const TPoint<TYPE, 3> &pt, TVec<TYPE, 3> *vec) {
	TPoint<TYPE, 3> ret = pt;

	if (ret.x < x) {
		ret.x = 2 * x - ret.x;
		if (vec) vec->x *= -1;
	} else if (ret.x > (x + width)) {
		auto xw = x + width;
		ret.x = 2 * xw - ret.x;
		if (vec) vec->x *= -1;
	}

	if (ret.y < y) {
		ret.y = 2 * y - ret.y;
		if (vec) vec->y *= -1;
	} else if (ret.y > (y + height)) {
		auto yh = y + height;
		ret.y = 2 * yh - ret.y;
		if (vec) vec->y *= -1;
	}

	if (ret.z < z) {
		ret.z = 2 * z - ret.z;
		if (vec) vec->z *= -1;
	} else if (ret.z > (z + depth)) {
		auto zd = z + depth;
		ret.z = 2 * zd - ret.z;
		if (vec) vec->z *= -1;
	}

	return ret;
}

template <typename TYPE>
TPoint<TYPE, 3> TCuboid<TYPE>::randomPoint() {
	TPoint<TYPE, 3> ret;

	ret.x = random(x * 1000, (x + width) * 1000) / 1000.0;
	ret.y = random(y * 1000, (y + height) * 1000) / 1000.0;
	ret.z = random(z * 1000, (z + depth) * 1000) / 1000.0;

	return ret;
}

/* ----------- Point System -------------- */


template <unsigned SIZE, typename POINT_TYPE>
PointSystem<SIZE, POINT_TYPE>::PointSystem(size_t maxSize) : points(maxSize) {}

template <unsigned SIZE, typename POINT_TYPE>
void PointSystem<SIZE, POINT_TYPE>::stopAll() {
	for (auto &point : points) {
		point.active = false;
	}
}

template <unsigned SIZE, typename POINT_TYPE>
void PointSystem<SIZE, POINT_TYPE>::setMaxPoints(size_t size, bool enableNewPoints) {
	if (size == points.size()) return;

	if (size < points.size()) {
		points.resize(size);
		return;
	}

	auto i = points.size();
	points.resize(size);

	for (; i < points.size(); i++) {
		points[i].active = enableNewPoints;
	}
}

template <unsigned SIZE, typename POINT_TYPE>
void PointSystem<SIZE, POINT_TYPE>::setVelocity(TVec<float, SIZE> vel) {
	for (auto &point : points) {
		point.vel = vel;
	}
}

template <unsigned SIZE, typename POINT_TYPE>
void PointSystem<SIZE, POINT_TYPE>::addBoundedPoints(BoundingType bounds, size_t pointCount, float velocity) {
	this->bounds = bounds;
	this->bounded = true;

	size_t i = 0;
	while (pointCount-- && i < points.size()) {
		auto &point = points[i];
		if (point.active) {
			i++;
			continue;
		}

		point.init();
		point.active = true;

		point.origin = bounds.randomPoint();
		point.vel.randomize(-10000, 10000);
		point.vel = points[i].vel.normalize();

		point.vel *= velocity;

		i++;
	}
}

template <unsigned SIZE, typename POINT_TYPE>
void PointSystem<SIZE, POINT_TYPE>::addPoints(int16_t count) {
	if (nextPointTime > millis()) return;
	nextPointTime = millis() + pointTimeout;

	if (count > 1 && pointTimeout > 0) count = 1;

	for (auto i = 0; i < points.size(); i++) {
		if (points[i].active) continue;

		points[i].init();
		points[i].active = true;
		return;

		if (count <= 0) return;
		count--;
	}
}

template <unsigned SIZE, typename POINT_TYPE>
uint32_t PointSystem<SIZE, POINT_TYPE>::activePointCount() {
	uint16_t ret = 0;
	for (auto i = 0; i < points.size(); i++) {
		if (points[i].active) ret++;
	}
	return ret;
}

template <unsigned SIZE, typename POINT_TYPE>
void PointSystem<SIZE, POINT_TYPE>::update() {
	if (activePointCount() < minPoints) {
		addPoints();
	}

	for (auto &point : points) {
		if (!point.active) continue;

		point.update();
		if (bounded) {
			point.origin = bounds.boundPoint(point.origin, &point.vel);
		}
	}
}

