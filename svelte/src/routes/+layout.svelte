<script lang="ts">
	import favicon from '$lib/assets/favicon.svg';

	import '@picocss/pico';
	import './app.css';

	let { children } = $props();
</script>

<svelte:head>
	<link rel="icon" href={favicon} />
</svelte:head>

<main class="container">
	{@render children()}
</main>
