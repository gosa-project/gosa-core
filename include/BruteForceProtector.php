<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2025 GONICUS GmbH
 *
 * ID: $$Id$$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

class BruteForceProtector
{
    private int $startDelayAfter;
    private int $maxDelaySec;
    private int $ttl;
    private int $rateLimitWindowSec;
    private int $maxAttemptsInWindow;

    /**
     * Creates the BruteForceProtector which starts delays after failed login attempts with same username and
     * only allows a certain amount of failed login attempts of the same ip within a specified time window.
     * @param CacheInterface $cache
     * @param int $ttl Time to live. Store value in the cache for ttl seconds. Default is 1h. Values <= 0 result in default.
     * @param int $startDelayAfter After how many failed login attempts with same username the delays should start. Default is 3. Values <= 0 result in default.
     * @param int $maxDelay Maximum delay in seconds after failed attempts with same username. Default is 30min. Values <= 0 result in default.
     * @param int $rateLimitWindow Length of the window in seconds, that should only allow limited number of failed attempts from same ip. Default is 5min. Values <= 0 result in default.
     * @param int $maxAttemptsPerWindow Maximum number of failed attempts per window. Default is 20. Values <= 0 result in default.
     */
    public function __construct(
        private CacheInterface $cache,
        int $ttl = 3600,
        int $startDelayAfter = 3,
        int $maxDelay = 1800,
        int $rateLimitWindow = 300,
        int $maxAttemptsPerWindow = 20
    )
    {
        $this->ttl = $ttl > 0 ? $ttl : 3600;
        $this->startDelayAfter = $startDelayAfter > 0 ? $startDelayAfter : 3;
        $this->maxDelaySec = $maxDelay > 0 ? $maxDelay : 1800;
        $this->rateLimitWindowSec = $rateLimitWindow > 0 ? $rateLimitWindow : 300;
        $this->maxAttemptsInWindow = $maxAttemptsPerWindow > 0 ? $maxAttemptsPerWindow : 20;
    }

    /**
     * Returns the current delay of the passed username or the ip address from this request, depending on which one is higher.
     * After the delay, at least one login attempt with same ip and same username will work again.
     * @param string|null $username Username whose delay should be checked. Null if only ip based delay should be checked.
     * @return int Username based delay or ip based delay, depending which one is higher
     */
    public function getCurrentDelay(string|null $username): int
    {
        // no delay if passed username is null or empty
        $usernameDelay = 0;

        if ($username !== null && $username !== '') {
            $usernameCacheKey = $this->getUsernameCacheKey($username);
            $usernameCacheValue = $this->getCacheValue($usernameCacheKey);
            $usernameDelay = $this->getUsernameDelay($usernameCacheValue);
        }

        // no delay if we do not get ip address from web server
        $ipDelay = 0;

        $ip = $_SERVER['REMOTE_ADDR'] ?? null;
        if ($ip !== null && $ip !== '') {
            $ipCacheKey = $this->getIpCacheKey($ip);
            $ipCacheValue = $this->getCacheValue($ipCacheKey);
            $ipDelay = $this->getIpDelay($ipCacheValue);
        }

        return max($usernameDelay, $ipDelay);
    }

    /**
     * Is called when a user logged in successfully, to reset failed login attempts.
     * @param string $username Username of the user who successfully logged in
     * @return void
     */
    public function onSuccess(string $username): void
    {
        $usernameCacheKey = $this->getUsernameCacheKey($username);
        $this->cache->delete($usernameCacheKey);

        $ip = $_SERVER['REMOTE_ADDR'] ?? null;
        if ($ip === null || $ip === '') {
            return;
        }

        $ipCacheKey = $this->getIpCacheKey($ip);
        $this->cache->delete($ipCacheKey);
    }

    /**
     * Is called when a user login failed, to increase the failed login attempts per username and ip.
     * @param string $username Username of the user who failed the login
     * @return void
     */
    public function onFailure(string $username): void
    {
        $usernameCacheKey = $this->getUsernameCacheKey($username);
        $usernameCacheValue = $this->getCacheValue($usernameCacheKey);

        $newUsernameCount = (int) $usernameCacheValue['count'] + 1;
        $currentTime = time();

        $newUsernameCacheValue = [
            'count' => $newUsernameCount,
            'timestamp' => $currentTime,
        ];

        $this->cache->set($usernameCacheKey, $newUsernameCacheValue, $this->ttl);

        $ip = $_SERVER['REMOTE_ADDR'] ?? null;
        if ($ip === null || $ip === '') {
            return;
        }

        $ipCacheKey = $this->getIpCacheKey($ip);
        $ipCacheValue = $this->getCacheValue($ipCacheKey);

        $ipCount = (int) $ipCacheValue['count'];
        $ipTimestamp = (int) $ipCacheValue['timestamp'];

        if ($ipTimestamp === 0 || $currentTime - $ipTimestamp >= $this->rateLimitWindowSec) {
            $ipTimestamp = $currentTime;
            $ipCount = 1;
        } else {
            $ipCount++;
        }

        $newIpCacheValue = [
            'count' => $ipCount,
            'timestamp' => $ipTimestamp,
        ];

        $this->cache->set($ipCacheKey, $newIpCacheValue, $this->ttl);
    }

    private function getUsernameCacheKey(string $username): string
    {
        $usernameNormalized = strtolower(trim($username));
        return 'bfp:username:' . hash('sha256', $usernameNormalized);
    }

    private function getIpCacheKey(string $ip): string
    {
        $ipNormalized = trim($ip);
        return 'bfp:ip:' . hash('sha256', $ipNormalized);
    }

    private function getCacheValue(string $key): array
    {
        $cacheValue = $this->cache->get($key);
        $default =  [
            'count' => 0,
            'timestamp' => 0,
        ];

        if ($cacheValue === null) {
            return $default;
        }

        if (!isset($cacheValue['count']) || !isset($cacheValue['timestamp'])) {
            return $default;
        }

        return $cacheValue;
    }

    private function getUsernameDelay(array $cacheValue): int
    {
        $count = (int) $cacheValue['count'];

        if ($count < $this->startDelayAfter) {
            return 0;
        }

        $timestamp = (int) $cacheValue['timestamp'];
        $waitedTime = time() - $timestamp;

        $punishableAttemptIndex = $count - $this->startDelayAfter;
        $possibleDelay = pow(2, $punishableAttemptIndex);

        $delay = min($this->maxDelaySec, $possibleDelay);
        
        $remainingDelay = (int) $delay - $waitedTime;

        return max(0, $remainingDelay);
    }

    private function getIpDelay(array $cacheValue): int
    {
        $count = (int) $cacheValue['count'];
        $timestamp = (int) $cacheValue['timestamp']; // start time of the current window

        // no delay if were under the max attemps
        if ($count < $this->maxAttemptsInWindow) {
            return 0;
        }

        $waitedTime = time() - $timestamp; // waited time since start of the current window

        // no delay if window is finished
        if ($waitedTime >= $this->rateLimitWindowSec) {
            return 0;
        }

        return $this->rateLimitWindowSec - $waitedTime;
    }
}