#include "pose.hpp"

#include <cmath>
#include <numbers>

Quaternion::Quaternion(double w, double x, double y, double z):
    w(w), x(x), y(y), z(z)
{}

Quaternion Quaternion::operator~() const
{
    return Quaternion(w, -x, -y, -z);
}

Quaternion Quaternion::operator-() const
{
    return Quaternion(-w, -x, -y, -z);
}

Quaternion Quaternion::operator+(const Quaternion& q) const
{
    return Quaternion(w + q.w, x + q.x, y + q.y, z + q.z);
}

Quaternion Quaternion::operator-(const Quaternion& q) const
{
    return Quaternion(w - q.w, x - q.x, y - q.y, z - q.z);
}

Quaternion Quaternion::operator*(const Quaternion& q) const
{
    return Quaternion(
        w * q.w - x * q.x - y * q.y - z * q.z,
        w * q.x + x * q.w + y * q.z - z * q.y,
        w * q.y - x * q.z + y * q.w + z * q.x,
        w * q.z + x * q.y - y * q.x + z * q.w
    );
}

Quaternion Quaternion::operator*(double k) const
{
    return Quaternion(k * w, k * x, k * y, k * z);
}

PoseTransform::PoseTransform(const Quaternion& rotation, const Quaternion& translation):
    rotation(rotation), translation(translation)
{}

PoseTransform::PoseTransform(const double euler[3]):
    rotation(0, 0, 0, 0),
    translation(0, 0, 0, 0)
{
    Quaternion rotationX = Quaternion(cos(euler[0] * std::numbers::pi / 360), sin(euler[0] * std::numbers::pi / 360), 0, 0);
    Quaternion rotationY = Quaternion(cos(euler[1] * std::numbers::pi / 360), 0, sin(euler[1] * std::numbers::pi / 360), 0);
    Quaternion rotationZ = Quaternion(cos(euler[2] * std::numbers::pi / 360), 0, 0, sin(euler[2] * std::numbers::pi / 360));
    rotation = rotationZ * rotationY * rotationX;
}

PoseTransform::PoseTransform(const double euler[3], const double pivot[3]): PoseTransform(euler)
{
    Quaternion q_pivot(0, pivot[0], pivot[1], pivot[2]);
    translation = q_pivot - rotation * q_pivot * ~rotation;
}

PoseTransform PoseTransform::operator*(const PoseTransform& t) const
{
    return PoseTransform(rotation * t.rotation, *this * t.translation);
}

Quaternion PoseTransform::operator*(const Quaternion& q) const
{
    return rotation * q * ~rotation + translation;
}