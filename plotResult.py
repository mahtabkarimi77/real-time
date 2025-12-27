import numpy as np
import matplotlib.pyplot as plt

# ---------- Load data ----------
# هر خط یک عدد
pr = np.loadtxt('/content/pr.txt', dtype=float)           # charge rate per minute
power = np.loadtxt('/content/powerInS.txt', dtype=float)  # battery level per minute

# اگر طول‌ها متفاوت بود، کوتاه‌تر را مبنا می‌گیریم
n = int(min(len(pr), len(power)))
pr = pr[:n]
power = power[:n]

t = np.arange(n)  # time index in minutes

# ---------- Helper metrics ----------
def moving_average(x, w=7):
    w = int(max(1, w))
    if w == 1:
        return x.copy()
    kernel = np.ones(w) / w
    # pad to keep same length
    pad = w // 2
    xpad = np.pad(x, (pad, pad), mode='edge')
    return np.convolve(xpad, kernel, mode='valid')

pr_ma = moving_average(pr, w=7)
power_ma = moving_average(power, w=7)

# تغییرات باتری در هر دقیقه (تقریب مشتق)
dp = np.diff(power, prepend=power[0])

# آمارهای ساده برای گزارش
stats = {
    "minutes": n,
    "battery_min": float(np.min(power)),
    "battery_max": float(np.max(power)),
    "battery_mean": float(np.mean(power)),
    "charge_min": float(np.min(pr)),
    "charge_max": float(np.max(pr)),
    "charge_mean": float(np.mean(pr)),
    "near_empty_minutes(<10)": int(np.sum(power < 10)),
    "near_full_minutes(>95)": int(np.sum(power > 95)),
}
print("Summary stats:")
for k, v in stats.items():
    print(f"- {k}: {v}")

# ---------- Plot 1: Battery level over time ----------
plt.figure()
plt.plot(t, power, linewidth=1)
plt.plot(t, power_ma, linewidth=2)
plt.title("Battery Level Over Time (per minute)")
plt.xlabel("Time (minute)")
plt.ylabel("Battery level")
plt.grid(True)
plt.show()

# ---------- Plot 2: Charge rate over time ----------
plt.figure()
plt.plot(t, pr, linewidth=1)
plt.plot(t, pr_ma, linewidth=2)
plt.title("Charging Rate Over Time (per minute)")
plt.xlabel("Time (minute)")
plt.ylabel("Charge rate")
plt.grid(True)
plt.show()

# ---------- Plot 3: Compare charge rate and battery on same timeline ----------
# برای اینکه مقیاس‌ها قابل مقایسه شوند، نرمال‌سازی می‌کنیم
pr_norm = (pr - pr.min()) / (pr.max() - pr.min() + 1e-9)
power_norm = (power - power.min()) / (power.max() - power.min() + 1e-9)

plt.figure()
plt.plot(t, power_norm, linewidth=2, label="Battery (normalized)")
plt.plot(t, pr_norm, linewidth=2, label="Charge rate (normalized)")
plt.title("Battery vs Charging Rate (Normalized Comparison)")
plt.xlabel("Time (minute)")
plt.ylabel("Normalized value")
plt.grid(True)
plt.legend()
plt.show()

# ---------- Plot 4: Battery change per minute (dp) ----------
plt.figure()
plt.plot(t, dp, linewidth=1)
plt.title("Battery Change Per Minute (ΔBattery)")
plt.xlabel("Time (minute)")
plt.ylabel("Δ Battery")
plt.grid(True)
plt.show()

# ---------- Plot 5: Scatter: Charge rate vs Battery change ----------
plt.figure()
plt.scatter(pr, dp, s=12)
plt.title("Scatter: Charge Rate vs Battery Change (ΔBattery)")
plt.xlabel("Charge rate")
plt.ylabel("Δ Battery")
plt.grid(True)
plt.show()

# ---------- Optional: Save figures if needed ----------
# plt.savefig("figure.png", dpi=200, bbox_inches="tight")
