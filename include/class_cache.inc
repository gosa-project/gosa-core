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

interface CacheInterface
{
    /**
     * Get stored value from the cache by key
     * @param string $key The key used to store the value
     * @return mixed The stored value on success. null on failure.
     */
    public function get(string $key): mixed;

    /**
     * Store the passed value the passed key in the cache
     * @param string $key The key used to store the value
     * @param mixed $value The value to store
     * @param int $ttl Time to live. Store value in the cache for ttl seconds.
     *  0 means the value will persists until its removed manually.
     * @return bool true on success. false on failure.
     */
    public function set(string $key, mixed $value, int $ttl = 0): bool;

    /**
     * Delete value from the cache by key
     * @param string $key The key of the value to delete
     * @return bool 
     */
    public function delete(string $key): bool;

    /**
     * Increment value in the cache by key
     * @param string $key The key of the value being increased
     * @param int $step The value to increase
     * @return int|false The stored value (after the incrementation) on success. false on failure.
     */
    public function increment(string $key, int $step = 1): int|false;
}


class ApcuCache implements CacheInterface
{
    public function get(string $key): mixed
    {
        $success = false;
        $value = apcu_fetch($key, $success);
        return $success ? $value : null;
    }

    public function set(string $key, mixed $value, int $ttl = 0): bool
    {
        return apcu_store($key, $value, $ttl);
    }

    public function delete(string $key): bool
    {
        return apcu_delete($key);
    }

    public function increment(string $key, int $step = 1): int|false
    {
        if (!apcu_exists($key)) {
            apcu_store($key, 0);
        }
        return apcu_inc($key, $step);
    }
}
